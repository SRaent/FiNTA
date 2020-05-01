
#define PRINT(x) cout << #x << " => " << x << endl;

#define SQRT2 (double)1.4142135623730950488016
#define PI (double)3.1415926535897932384626433832795



/* DONE;
allow for scaling units
Tracing data
Loop Data
Animation
fix connection between existing nodes
draw name or ending not specified
write shell script to apply programm to each file in folder
*/

/* TODO:
print statistics about local maxima of smoothed angfun
*/





//#define RV 8 // 8 vision radius (~2 times fibre thickness)
//#define RM 0 //minimum vision radius
//#define RS 3 //step radius
//#define STEPS 360
//#define DEV 0.5 // 0.55 deviation of gaussian smooth of circlefun
//#define TH 2 //0.5 threshold for findpks
//#define ML 7 //minimum loop length
//#define SG 1.8 //1.8 ; 2.3 (a bit less than half fibre thickness)






//computed
#define RN (RF+RS) //neighbor radius
#define RF (RS/SQRT2)  //forbidden radius


//nolonger used
#define RT RV// vision for threshold
#define LT 2 // line thiccness for connectable test
#define LD 0.2 //deviation of smoothing if line function for connectable test
#define LS RS*LT // steps for averaging the line function
#define CT 3 // Connectabel threshhold. if the smoothed function goes below this, no new node will be spawned.
#define FA 0.125*PI //forbidden angle for already connected nodes to spawn new ones


#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <complex.h>
//#include <fftw3.h>
#include <thread>

#include <random>

//#include "tiffio.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgcodecs.hpp"


using namespace std;
using namespace cv;

//#include "datafun.h"
//#include "datafun.cpp"
#include "userinterface.cpp"
#include "mathfun.cpp"
#include "node.h"
#include "node.cpp"
#include "analyse.cpp"
#include "visualise.cpp"
#include "generate.cpp"
#include "united_junction.h"
#include "united_junction.cpp"


int main(int n, char** args){
	
	bool settings_read = false;
	bool image_specified = false;
	
	for(int i = 1; i < n; ++i){
		//cout << args[i] << " " << i << endl;
		if(strcmp(args[i],"-s") == 0 && ++i < n){
			if(!settings_read){
				if (read_settings(args[i])){
					settings_read = true;
					cout << "\nSettings read successfully" << endl;
				}
				else {
					cout << "ERROR: Settings could not be read" << endl;
					return -1;
				}
			}
			else {
				cout << "ERROR: Settings already read" << endl;
			}
		}
		else if(strcmp(args[i],"-f") == 0 && ++i < n){
			
			if(!split_path(args[i], folder, file, ending)){
				cout << "ERROR: Path to image: \"" + string(args[i]) + "\" appers to be a directory" << endl;
				return -1;
			}
			else{
				cout << "analyzing image: " << file+ending << endl;
			}
			//cout << "\nfolder: " + folder + "\nfile: " + file + "\nending: " + ending << endl;
		}
		else {
			cout << "ERROR: argument \"" << args[i] << "\" could not be interpreted" << endl;
			return -1;
		}
	}
	
	if (!(RV_set && RS_set && DEV_set && SG_set)){
		cout << "ERROR: not all necessary parameters were specified" << endl;
		return -1;
	}
	
	
	for (unsigned long long i = 0; i < draw_commands.size(); ++i){
		if (!draw_commands[i]->name_specified){
			draw_commands[i]->name = "<imagename>_" + draw_commands[i]->background;
		}
		if (draw_commands[i]->ending == ""){
			draw_commands[i]->ending = ending;
		}
	}
	
	
	
	
	
	
	Mat I1 = imread(folder+file+ending);
	if ( !I1.data){
		cout << "ERROR: Image \"" << folder + file + ending << "\" could not be opened" << endl;
		return -1;
	}
	Size s = I1.size();
	Size s_orig = I1.size();
	//PRINT(s.width);
	//PRINT(s.height);
	Mat I2;
	
	if (cropped && (crop_x1 - crop_x2) * (crop_y1 - crop_y2) == 0){
		cout << "ERROR: specified cropping recktangle has no area" << endl;
		return -1;
	}
	else if (cropped && (s.width < crop_x1 || s.width < crop_x2 || s.height < crop_y1 || s.height < crop_y2)){
		cout << "ERROR: Intended cropping recktangle does not fit inside image" << endl;
		return -1;
	}
	if (cropped){
		unsigned long long x1 = crop_x1;
		unsigned long long x2 = crop_x2;
		unsigned long long y1 = crop_y1;
		unsigned long long y2 = crop_y2;
		
		crop_x1 = min(x1,x2);
		crop_x2 = max(x1,x2);
		crop_y1 = min(y1,y2);
		crop_y2 = max(y1,y2);
		
		Rect myROI(crop_x1,crop_y1,crop_x2-crop_x1,crop_y2 - crop_y1);
		I2 = I1(myROI);
	}
	else {
		I2 = I1;
	}
	if (add_noise != 0){
		I2 = noisify_gauss_absolute(I2, add_noise);
	}
	s = I2.size();
	//PRINT(s.width);
	//PRINT(s.height);
	Mat I3;
	I2.copyTo(I3);
	//PRINT(I3.channels());
	cvtColor(I3,I3,COLOR_BGR2GRAY);
	//PRINT(I3.channels());
	Mat I4;
	I3.convertTo(I4, CV_64F);
	Mat I_orig_double;
	I1.copyTo(I_orig_double);
	cvtColor(I_orig_double,I_orig_double,COLOR_BGR2GRAY);
	I_orig_double.convertTo(I_orig_double, CV_64F);
	/*
	unsigned long long min;
	unsigned long long max;
	minMaxLoc(I3, &min, &max);
	if (node_thresh > min){
		node_thresh = (node_thresh - min)*255/(max - min);
	}
	*/
	normalize(I4,I4,255,0,32); // the 32 is a flag, that produced errors if the keyword is used directly
	
	Mat hessian;
	if (modified_hessian){
		hessian = convolve_modified_hessian(I4,SG*30,SG);
	}
	else{
		hessian = convolve_hessian(I4,SG*30,SG);
	}
	Mat tubeness = tubeness_hessian(hessian);
	Mat viz_hessian = visualize_hessian(hessian);
	
	//PRINT(viz_hessian.channels())
	viz_hessian.convertTo(viz_hessian,CV_8UC3);
	
	vector<node*> list;
	vector<node**> closures;
	
	if (auto_start_points){
		if (!rad_startpoints_specified){
			rad_startpoints = 20.0 * RS;
		}
		gen_startpoints(list, closures, hessian, tubeness, n_startpoints, rad_startpoints, thresh_startpoints);
		if (list.size() == 0){
			cout << "WARNING: the automated start point generation did not generate any starting points, this is likely due to a to big threshold" << endl;
		}
		else {
			cout << "\n" << list.size() << " startpoint(s) generated automatically" << endl;
		}
	}
	
	for ( unsigned long long i = 0; i < man_start_points.size(); ++i){
		if (man_start_points[i]->x <= s.width && man_start_points[i]->y <= s.height){
			new node(man_start_points[i]->x,man_start_points[i]->y, &list, &closures, &hessian);
		}
		else {
			cout << "ERROR: Manually placed startpoint lies outside the (cropped) image" << endl;
			return -1;
		}
	}
	
	if (list.size() == 0){
		cout << "WARNING: No starting points were created, therefore no tracing will take place" << endl;
	}
	
	
	normalize(tubeness,tubeness,255,0,32);
	tubeness.convertTo(tubeness,CV_8UC3);
	
	
	
	for (unsigned long long i = 0; i < draw_commands.size(); ++i){
		if ( draw_commands[i]->background == "original"){
			I1.copyTo(draw_commands[i]->image);
			draw_commands[i]->cropped = false;
		}
		else if ( draw_commands[i]->background == "cropped"){
			I2.copyTo(draw_commands[i]->image);
			draw_commands[i]->cropped = true;
		}
		else if ( draw_commands[i]->background == "tubeness"){
			tubeness.copyTo(draw_commands[i]->image);
			draw_commands[i]->cropped = true;
		}
		else if ( draw_commands[i]->background == "hessian"){
			hessian.copyTo(draw_commands[i]->image);
			normalize(draw_commands[i]->image,draw_commands[i]->image,255,0,32);
			draw_commands[i]->cropped = true;
		}
		else if ( draw_commands[i]->background == "visualized_hessian"){
			viz_hessian.copyTo(draw_commands[i]->image);
			draw_commands[i]->cropped = true;
		}
		else if ( draw_commands[i]->background == "white"){
			Mat tmp(s.height,s.width, CV_8UC3, Scalar::all(255));
			tmp.copyTo(draw_commands[i]->image);
			draw_commands[i]->cropped = true;
		}
		else if ( draw_commands[i]->background == "black"){
			Mat tmp(s.height,s.width, CV_8UC3, Scalar::all(0));
			tmp.copyTo(draw_commands[i]->image);
			draw_commands[i]->cropped = true;
		}
		else {
			cout << "something went terribly wrong, please send me the config file you used via moritz.schu@t-online.de" << endl;
			return -1;
		}
	}
	
	
	
	
	
	
	
	unsigned long long i = 0;
	
	if (list.size() != 0){
		cout << endl << "Started tracing. This might take several minutes. Every iteration a \"i\" will be printed in the command line to see if the programm is still running" << endl;
	}
	
	
	
	//VideoWriter animation(animation_path, VideoWriter::fourcc('P','I','M','1') , 3, s_orig);
	
	VideoWriter animation;
	//int codec = VideoWriter::fourcc('P','I','M','1');
	//double fps = 3;
	if (animation_path != ""){
		int codec = animation.fourcc('P','I','M','1');  // select desired codec (must be available at runtime)
		double fps = 20.0;                          // framerate of the created video stream
		animation.open(replace_keywords(animation_path), codec, fps, s_orig, true);
		if (!animation.isOpened()){
			cout << "WARNING: animation can not be saved to path: \"" << animation_path << "\"" << endl;
		}
	}
	//animation.open(animation_path, VideoWriter::fourcc('P','I','M','1'), 3, s_orig);
	
	
	
	/*
	//--- INITIALIZE VIDEOWRITER
    VideoWriter writer;
    int codec = VideoWriter::fourcc('P','I','M','1');  // select desired codec (must be available at runtime)
    double fps = 25.0;                          // framerate of the created video stream
    //string filename = "./live.avi";             // name of the output video file
    //PRINT(replace_keywords(animation_path))
    writer.open(replace_keywords(animation_path), codec, fps, s_orig, true);
	*/
	
	
	for (bool buisy = 1; buisy && list.size() != 0;){
		buisy = 0;
		unsigned long long end = list.size();
		for (unsigned long long it = 0; it < end; ++it){
			if (!(list[it]->procreated)){
				if (del_worst_sect){
					list[it]->procreate_hessian_rate();//this is the whole spiel basically
				}
				else {
					list[it]->procreate_hessian();
				}
				buisy = 1;
			}
		}
		
		if (animation_path != ""){
			Mat frame;
			I1.copyTo(frame);
			frame = draw_list_offset(frame,list,animation_color, animation_thickness, crop_x1, crop_y1);
			animation.write(frame);
		}
		
		
		
		//for (unsigned long long i = 0; i < list.size(); ++i){
		//	PRINT(list[i])
		//}
		//cout << i++ << " " << list.size() << endl;
		cout << "i" << flush;
		
	}
	
	if (list.size() != 0){
		cout << endl << "Tracing finished" << endl;
	}

	
	
	

	/*
	cout << "testing" << endl;
	Size s2 = I_orig_double.size();
	PRINT(s2.width)
	PRINT(s2.height)
	PRINT(I_orig_double.at<double>(s2.height - 1,s2.width - 1))
	*/
	
	
	if (del_nodes_above_thresh){
		unsigned long long deleted = 0;
		for (unsigned long long i = 0; i < list.size(); ++i){
			if (list[i]->brightness(I_orig_double, crop_x1, crop_y1) > node_thresh_above){ delete list[i]; ++ deleted;}
		}
		cout << "Deleted " << deleted << " nodes above threhold" << endl;
	}
	if (del_nodes_below_thresh){
		unsigned long long deleted = 0;
		for (unsigned long long i = 0; i < list.size(); ++i){
			if (list[i]->brightness(I_orig_double, crop_x1, crop_y1) < node_thresh_below){ delete list[i]; ++ deleted;}
		}
		cout << "Deleted " << deleted << " nodes below threhold" << endl;
	}
	if (del_lines_below_thresh){
		unsigned long long deleted = 0;
		for (unsigned long long i = 0; i < list.size(); ++i){
			if (list[i]->brightness(I_orig_double, crop_x1, crop_y1) < node_line_thresh_below){deleted += list[i]->del_til_junc();}
		}
		cout << "Deleted " << deleted << " nodes below threhold along whole lines" << endl;
	}
	if (del_lines_above_thresh){
		unsigned long long deleted = 0;
		for (unsigned long long i = 0; i < list.size(); ++i){
			if (list[i]->brightness(I_orig_double, crop_x1, crop_y1) > node_line_thresh_above){deleted += list[i]->del_til_junc();}
		}
		cout << "Deleted " << deleted << " nodes above threhold along whole lines" << endl;
	}
	
	//experiment did not work, optimising the node positions after initial tracing was futile
	/*
	
	cout << "experiment starting" << endl;
	
	for (unsigned long long i = 0; i < list.size(); ++i){
		list[i]->x_orig = list[i]->x;
		list[i]->y_orig = list[i]->y;
	}
	
	unsigned long long iterations = 50;
	unsigned long long positions = 50;
	double thic = 2*SG;
	PRINT(thic);
	
	for (unsigned long long i = 0; i < iterations; ++i){
		for (unsigned long long j = 0; j < list.size(); ++j){
			list[j]->wiggle(positions, thic);
		}
		PRINT(i);
	}
	
*/	
	
	
	cout << "checking network integrety" << endl;
	for (const auto& n:list){
		for (const auto& c:n->connections){
			if (!n->is_in(c->connections) || !c->is_in(list)){
				cout << "we have a problem" << endl;
			}
		}
		/*
		if (abs(322.5 - n->x) < 1 && abs(657.0 - n->y) < 1){
			cout << "ladies and gentleman" << endl;
			cout << "we got him" << endl;
		}
		*/
	}
	for (const auto& c:closures){
		if (!c[0]->is_in(list) || !c[1]->is_in(list) || !c[0]->is_in(c[1]->connections) || !c[1]->is_in(c[0]->connections)){
			cout << "closures are fucked" << endl;
		}
	}
	//return 0;
	
	Mat testimg;
	testimg = draw_scale_list(I2,list,10);
	imwrite("test1.png",testimg);
	testimg = draw_closures_scaled(testimg,closures,10);
	testimg = draw_dots_scaled(testimg,list,10);
	imwrite("test2.png",testimg);
	//return 0;
	
	
	
	vector<double> aux_data;
	vector<string> aux_labels;
	aux_data.push_back(list.size());
	aux_labels.push_back("total number of nodes");
	aux_data.push_back(find_junctions(list).size());
	aux_labels.push_back("total number of junctions");
	aux_data.push_back(scaling_factor*network_length(list));
	aux_labels.push_back("total network lengt");
	
	for (unsigned long long i = 0; i < draw_commands.size(); ++i){
		if (draw_commands[i]->all_nodes){
			if (draw_commands[i]->cropped){
				draw_commands[i]->image = draw_list(draw_commands[i]->image,list,draw_commands[i]->all_nodes_color,draw_commands[i]->all_nodes_thickness);
			}
			else {
				draw_commands[i]->image = draw_list_offset(draw_commands[i]->image,list,draw_commands[i]->all_nodes_color,draw_commands[i]->all_nodes_thickness,crop_x1,crop_y1);
			}
		}
	}



	arrange_unite_vals_all(junc_dist_all);
	arrange_unite_vals_all(junc_conn_all);

	for (auto it = junc_opt::unite_vals_all.begin(); it != junc_opt::unite_vals_all.end(); ++it){
		cout << str_add_double("analysing united junction discances with unification distance " ,(*it)) << endl;
		vector<united_junction*> united_junctions = find_united_junctions(list,(*it));
		aux_labels.push_back(str_add_double("total number of united junctions with unification distance " ,(*it)));
		aux_data.push_back(united_junctions.size());

		Mat img;
		I2.copyTo(img);
		img = draw_list(img,list,Scalar(0,0,0),1);
		for (auto const& uni_junc: united_junctions){
			uint8_t c1 = rand()%255;
			uint8_t c2 = rand()%255;
			uint8_t c3 = rand()%255;
			Scalar color = Scalar(c1,c2,c3);
			//Scalar color2 = Scalar(c1,c2,c3);
			Scalar color2 = Scalar(0,0,255);
			img = draw_list(img,uni_junc->nodes,color);
			img = draw_closures(img,uni_junc->outgoing_connections,color2);
		}
		imwrite(str_add_double("junc_vis",*it)+".png",img);

		for ( auto jt = junc_dist_all.begin(); jt != junc_dist_all.end(); ++jt){
			if ((*jt)->unite == (*it)){
				(*jt)->data_floating = united_junction_distances(united_junctions);
			}
		}
		for (auto jt = united_junctions.begin(); jt != united_junctions.end(); ++jt){
			for ( auto lt = junc_conn_all.begin(); lt != junc_conn_all.end(); ++lt){
					if ((*lt)->unite == (*it)){
						(*lt)->data_integer.push_back((*jt)->outgoing_connections.size());
					}
			}
			delete (*jt);
		}
	}

	for (const auto& line_opt:line_analysis::line_options){
		if(!line_opt->on_only_loops){
			line_opt->process(I2, list);
		}
	}


/*

	// this can be very inefficient since if a unite value is chosen for both distances and connectivities, the junctions are determined twice
	for (auto it = junc_dist_all.begin(); it != junc_dist_all.end(); ++it){
		cout << "analysing united junction discances with unification distance " << (*it)->unite << endl;
		vector<united_junction*> united_junctions = find_united_junctions(list,(*it)->unite);
		(*it)->data_floating = united_junction_distances(unite_junctions);
		aux_data.push_back(united_junctions.size());
		aux_labels.push_back("number of unified junctions with unification distance " << (*it)->unite);
		for (auto jt = united_junctions.begin(); jt =! united_junctions.end(); ++jt){
			delete (*jt);
		}
	}
	
	for (auto it = junc_conn_all.begin(); it != junc_conn_all.end(); ++it){
		cout << "analysing united junction connectivities with unification distance " << (*it)->unite << endl;
		vector<united_junction*> united_junctions = find_united_junctions(list,(*it)->unite);

		for (auto jt = united_junctions.begin(); jt =! united_junctions.end(); ++jt){
			(*it)->data_integer.push_back() = (*jt)->outgoing_connections.size();
			delete (*jt);
		}
	}
*/
	

	
	if (tracing_data_path != ""){
		save_tracing_data(list, tracing_data_path);
	}
	
	
	
	
	cout << "extracting all closed loops from the traced network" << endl;
	
	
	only_loops(list, closures);
	for (const auto& c:closures){
		if((!c[0]->is_in(c[1]->connections)) || (!c[1]->is_in(c[0]->connections)) ){
			cout << "false closure not removed" << endl;
		}
	}
	
	if (list.size() == 0){
		cout << "WARNING: no loops present in the traced network" << endl;
		for (unsigned long long i = 0; i < draw_commands.size(); ++i){
			//Mat test(s.height,s.width, CV_8UC3, Scalar::all(255));
			//cout << "TEST " << replace_keywords(draw_commands[i]->folder+draw_commands[i]->name+draw_commands[i]->ending) << endl;
			if(!imwrite(replace_keywords(draw_commands[i]->folder+draw_commands[i]->name+draw_commands[i]->ending),draw_commands[i]->image)){
				cout << "WARNING: File: \"" << replace_keywords(draw_commands[i]->folder+draw_commands[i]->name+draw_commands[i]->ending) << "\" could not be opened" << endl;
				return -1;
			}
			if (aux_data_path != ""){
				save_aux_data(aux_data,aux_labels,replace_keywords(aux_data_path));
			}
		}
		return 0;
	}
	
	vector<vector<node*>> loops = find_loops(closures);

	
	
	cout << "Done extracting loops" << endl;
	
	
	
	aux_data.push_back(list.size());
	aux_labels.push_back("total number of nodes contributing to closed loops");
	aux_data.push_back(find_junctions(list).size());
	aux_labels.push_back("total number of junctions in the network of closed loops");
	aux_data.push_back(loops.size());
	aux_labels.push_back("total number of closed loops");
	aux_data.push_back(scaling_factor*scaling_factor*total_loop_area(loops));
	aux_labels.push_back("total area of all loops");
	aux_data.push_back(scaling_factor*network_length(list));
	aux_labels.push_back("total network length of closed loop network");
	


	if (loop_data_path != ""){
		save_loop_data(loops, loop_data_path);
	}

	if (circleness_path != ""){
		vector<double> c = loops_circleness(loops);
		double_vector_to_file(replace_keywords(circleness_path),c);
	}
	
	arrange_unite_vals_loop(junc_dist_loop);
	arrange_unite_vals_loop(junc_conn_loop);
	//PRINT(junc_opt::unite_vals_loop.size())

	for (auto it = junc_opt::unite_vals_loop.begin(); it != junc_opt::unite_vals_loop.end(); ++it){
		cout << str_add_double("analysing united junctions in the network only containing loops with unification distance " ,(*it)) << endl;
		vector<united_junction*> united_junctions = find_united_junctions(list,(*it));
		aux_labels.push_back(str_add_double("total number of united junctions the network only containing closed loops with unification distance ",(*it)));
		aux_data.push_back(united_junctions.size());
		for ( auto jt = junc_dist_loop.begin(); jt != junc_dist_loop.end(); ++jt){
			if ((*jt)->unite == (*it)){
				(*jt)->data_floating = united_junction_distances(united_junctions);
			}
		}
		for (auto jt = united_junctions.begin(); jt != united_junctions.end(); ++jt){
			for ( auto lt = junc_conn_loop.begin(); lt != junc_conn_loop.end(); ++lt){
					if ((*lt)->unite == (*it)){
						(*lt)->data_integer.push_back((*jt)->outgoing_connections.size());
					}
			}
			delete (*jt);
		}
	}
	for (const auto& line_opt:line_analysis::line_options){
		if(line_opt->on_only_loops){
			line_opt->process(I2, list);
		}
	}
	if (aux_data_path != ""){
		save_aux_data(aux_data,aux_labels,replace_keywords(aux_data_path));
	}
/*	
	if (junc_dist_loop_path != ""){
		
		cout<< "analysing Junctions in the network of nodes that are part of a loop" << endl;
		vector<node*> junctions = find_junctions(list);
		vector<double> jd =  junction_distances(junctions);
		
		double_vector_to_file(replace_keywords(junc_dist_loop_path),jd);
		
		cout<< "done analysing Junctions" << endl;
	}
	
	//PRINT(junc_conn_loop_path)
	if (junc_conn_loop_path != ""){
		//PRINT(junc_conn_loop_path)
		vector<node*> junctions = find_junctions(list);
		vector<double> jc;
		
		for (unsigned long long i = 0; i < junctions.size(); ++i){
			jc.push_back(junctions[i]->connections.size());
		}
		
		double_vector_to_file(replace_keywords(junc_conn_loop_path),jc);
		
	}*/
	save_junc_data(junc_dist_all);
	save_junc_data(junc_dist_loop);
	save_junc_data(junc_conn_all);
	save_junc_data(junc_conn_loop);
	
	
	for (unsigned long long i = 0; i < loop_area_settings.size(); ++i){
		vector<double> areas = find_loop_areas_w_diam(loops,loop_area_settings[i]->thickness);
		double_vector_to_file(replace_keywords(loop_area_settings[i]->path),areas);
	}
	
	if (loop_circumference_path != ""){
		double_vector_to_file(replace_keywords(loop_circumference_path),find_loop_lengths(loops));
	}
	
	
	for (unsigned long long i = 0; i < draw_commands.size(); ++i){
		if (draw_commands[i]->only_loops){
			if (draw_commands[i]->cropped){
				draw_commands[i]->image = draw_list(draw_commands[i]->image,list,draw_commands[i]->only_loops_color,draw_commands[i]->only_loops_thickness);
			}
			else {
				draw_commands[i]->image = draw_list_offset(draw_commands[i]->image,list,draw_commands[i]->only_loops_color,draw_commands[i]->only_loops_thickness,crop_x1,crop_y1);
			}
		}
	}
	
	for (unsigned long long i = 0; i < draw_commands.size(); ++i){
		if (!imwrite(replace_keywords(draw_commands[i]->folder+draw_commands[i]->name+draw_commands[i]->ending),draw_commands[i]->image)){
			cout << "WARNING: File: \"" << replace_keywords(draw_commands[i]->folder+draw_commands[i]->name+draw_commands[i]->ending) << "\" could not be opened" << endl;
		}
	}
	
	
	return 0;
}




// if you wish to tinker with the code, use this main function here.
int main_tinker(){
	
	//Mat I2 = imread("/full/path/to/image.tif"); // use if you dont want to input the file and folder seperately
	folder = "/folder/containing/images/";
	file = "imagename.tif";
	string write_folder = "./"; //Folder where output will be written
	
	
	
	
	Mat I2 = imread(folder+file+".tif");
	if(!I2.data){
		cout << "ERROR: Image could not be importet" << endl;
		return -1;
	}
	//Mat I2(img);
	Rect myROI(0,0,I2.size().width,I2.size().height); // Here the rectangle used to crop the image is defined
	//Rect myROI(0,0,1024,884); // Here the rectangle used to crop the image is defined
	Mat I3 = I2(myROI);
	cv::cvtColor(I3, I3, cv::COLOR_BGR2GRAY);
	Size s = I2.size();
	Mat I4;
	I3.copyTo(I4);
	Mat I5;
	I3.convertTo(I5, CV_64F);
	
	
	normalize(I5,I5,255,0,32);
	
	
	PRINT(I5.channels());
	
	
	Mat hessian = convolve_hessian(I5,50,SG); //1.8 ; 2.3 (a bit less than half fibre thickness)
	Mat tubeness = tubeness_hessian(hessian);
	
	
	
	vector<node*> list;
	vector<node**> closures;
	gen_startpoints(list, closures, hessian, tubeness);
	
	normalize(tubeness,tubeness,255,0,32);
	
	
	
	unsigned long long i = 0;
	
	//cout << write_folder+file+"_junction_dist.dat" << endl;
	for (bool buisy = 1; buisy && i < 500 ;){
		buisy = 0;
		unsigned long long end = list.size();
		for (unsigned long long it = 0; it < end; ++it){
			if (!(list[it]->procreated)){
				list[it]->procreate_hessian();
				buisy = 1;
			}
		}
		
		//for (unsigned long long i = 0; i < list.size(); ++i){
		//	PRINT(list[i])
		//}
		cout << i++ << " " << list.size() << endl;
		
	}

	
	
	/*
	vector<node*> junctions = find_junctions(list);
	cout<< "analysing Junctions" << endl;
	vector<double> jd =  junction_distances(junctions);
	
	double_vector_to_file(write_folder+file+"_junction_dist.dat",jd);
	
	cout<< "done analysing Junctions" << endl;
	*/
	I3.convertTo(I3, CV_8U);
	cv::cvtColor(I3, I3, cv::COLOR_GRAY2BGR);
	double line_thickness = 2.4;
	I3 = draw_list(I3,list,Scalar(255,0,0),line_thickness);
	
	PRINT(list.size())
	
	only_loops(list, closures);
	
	I3 = draw_list(I3,list,Scalar(0,0,255),line_thickness);
	
	PRINT(list.size())
	
//	I3 = draw_list(I3,junctions,Scalar(0,0,0));
	
	
	vector<vector<node*>> loops = find_loops(closures);
	
	vector<double> areas = find_loop_areas_wo_max_w_diam(loops,2.4);
	double_vector_to_file(write_folder+file+"_areas.dat",areas);
	//PRINT(list.size())
	PRINT(loops.size())
	PRINT(closures.size())
	
	
	
//	I3 = draw_closures(I3,closures,Scalar(128,0,128));
	
	//draw_loops("./lines/", lines, I3, true);
	cout << "total area: " << total_loop_area(loops) << " max area: " << max_loop_area(loops) << " difference: " << max_loop_area(loops) - total_loop_area(loops)  << endl;
	
	imwrite(write_folder+file+"_tracing.tif",I3);
	imwrite(write_folder+file+"_wo_scalebar.tif",I4);
	return 0;
}
