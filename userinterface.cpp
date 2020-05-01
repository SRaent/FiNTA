#ifndef USERINTERFACE_CPP
#define USERINTERFACE_CPP USERINTERFACE_CPP
	
#include <limits> 
#include <string>
#include <stdlib.h>
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


using namespace std;
using namespace cv;

double scaling_factor = 1.0;


//#include "datafun.h"
//#include "datafun.cpp"
#include "node.h"
#include "united_junction.h"


unsigned long long THREADS = 1;
bool THREADS_set = false;

double RV = 8;						// 8 vision radius (~2 times fibre thickness)
double RM = 0;						//minimum vision radius
double RS = 3;						//step radius (~ 0.5 - 1 x fiber thickness)
unsigned long long STEPS = 360;		//number of steps the smoothing function is computed for
double DEV = 0.5;					// 0.55 deviation of gaussian smooth of circlefun
double TH = 0;						//0.5 threshold for findpks
unsigned long long ML = 6;			//minimum loop length
double SG = 1.8;					//1.8 ; 2.3 (a bit less than half fibre thickness)


//computed
#define RN (RF+RS) //neighbor radius
#define RF (RS/SQRT2)  //forbidden radius

bool RV_set = false;
bool RM_set = false;
bool RS_set = false;
bool STEPS_set = false;
bool DEV_set = false;
bool TH_set = false;
bool ML_set = false;
bool SG_set = false;



bool cropped = false;
unsigned long long crop_x1 = 0;
unsigned long long crop_y1 = 0;
unsigned long long crop_x2 = 0;
unsigned long long crop_y2 = 0;


unsigned long long node_thresh_below = 0;
unsigned long long node_thresh_above = 0;
bool del_nodes_below_thresh = false;
bool del_nodes_above_thresh = false;


unsigned long long node_line_thresh_below = 0;
unsigned long long node_line_thresh_above = 0;
bool del_lines_below_thresh = false;
bool del_lines_above_thresh = false;


bool modified_hessian = false;

bool start_points_exist = false;
vector<Point*> man_start_points;

bool auto_start_points = false;
unsigned long long n_startpoints = 100;
double rad_startpoints = 20*RS;
bool rad_startpoints_specified = false;
double thresh_startpoints = 0;

//i suppose this i why one uses header files...
bool is_number(string, double&);
bool is_number(string);
bool is_number(string, unsigned long long&);
vector<vector<node*>> find_lines(vector<node*>,double);
void double_vector_to_file(string, vector<double>);
vector<double> line_lengths(vector<vector<node*>>);
void draw_lines(const Mat, const vector<vector<node*>>, const string, const double, const double);
string replace_keywords(string);

class line_analysis{
	public:
	static vector<line_analysis*> line_options;
	double angle = 3.14159*22.5/180.0;
	bool angle_set = false;
	bool visualize = false;
	bool on_only_loops = false;
	string data_path = "<imagename>_line_lengths.dat";
	bool data_path_set = false;
	unsigned long long imgnum = 10;
	bool imgnum_set = false;
	string images_folder = "./";
	line_analysis(vector<string> line, bool& successful){
		if (line[0] == "save_loop_line_lengths"){ on_only_loops = true;}
		for (unsigned long long i = 1; i < line.size(); ++i){
			if (is_number(line[i],angle)){
					if(angle_set){
						cout << "line angle can only be set once per line analysis";
						successful = false;
						return;
					}
					else {angle_set = true;}
			}
			else if (line[i] == "draw_lines" && !visualize){
				visualize = true;
				if (i + 1 < line.size()){
					++i; //the = is correct here
					if (!(imgnum_set = is_number(line[i], imgnum))){
						images_folder = line[i];
					}
					if (i + 1 < line.size()){
						if (imgnum_set){
							++i;
							images_folder = line[i];
						}
						else if (is_number(line[i+1], imgnum)){
							++i;
						}
					}
				}

			}
			else if (!data_path_set){
				data_path_set = true;
				data_path = line[i];
			}
			else{
				successful = false;
				return;
			}
			
		}
		if (!angle_set) {successful = false;}
	}
	void process(Mat I, vector<node*> list){
		vector<vector<node*>> lines = find_lines(list,angle);
		vector<double> linelengths = line_lengths(lines);
		double_vector_to_file(replace_keywords(data_path),linelengths);
		if(visualize){
			draw_lines(I,lines,images_folder,imgnum,1);
		}
	}
};
vector<line_analysis*> line_analysis::line_options;

class draw_command{
	public:
	bool all_nodes = false;
	bool only_loops = false;
	Scalar all_nodes_color = Scalar(255,0,0);
	Scalar only_loops_color = Scalar(0,0,255);
	double all_nodes_thickness = 1;
	double only_loops_thickness = 1;
	
	string background = "original";
	bool background_specified = false;
	
	string folder = "";
	string name = "";
	string ending = "";
	bool name_specified = false;
	
	Mat image;
	
	bool cropped = false;
};



vector<draw_command*> draw_commands;


class save_loops{
	public:
	double thickness = 0;
	string path = "<imagename>_loop_areas.dat";
};

vector<save_loops*> loop_area_settings;


string loop_circumference_path = "";

//this is only nessesary because i am a shit programmer in my next project i am gonna do things right
void double_vector_to_file(string,vector<double>);
void ull_vector_to_file(string,vector<unsigned long long>);
bool is_number(string,unsigned long long&);
string replace_keywords(string);
class junc_opt{
	public:
	string path;
	unsigned long long unite = 0;
	vector<double> data_floating;
	vector<unsigned long long> data_integer;
	
	static vector<unsigned long long> unite_vals_all;
	static vector<unsigned long long> unite_vals_loop;
	//static vector<vector<junc_opt*>*> unite_val_settings_all;
	//static vector<vector<junc_opt*>*> unite_val_settings_loop;
	
	junc_opt(vector<string> w, string default_path, bool& successful){
		path = default_path;
		if (w.size() == 3){
			if(is_number(w[1],unite)){
				path = w[2];
			}
			else if (is_number(w[2],unite)){
				path = w[1];
			}
			else{ cout << w[0] << " arguments could not be interpretetd as a number and a path" << endl; successful = false;}
		}
		else if (w.size() == 2){
			if(!is_number(w[1],unite)){
				path = w[1];
			}
		}
	}
	void save_data(){
		if(data_integer.size() == 0){
			double_vector_to_file(replace_keywords(path),data_floating);
		}
		else {
			ull_vector_to_file(replace_keywords(path),data_integer);
		}
	}
};
vector<unsigned long long> junc_opt::unite_vals_all;
vector<unsigned long long> junc_opt::unite_vals_loop;

void arrange_unite_vals(vector<junc_opt*> junc_opts, vector<unsigned long long>& unite_vals){
	for (auto it = junc_opts.begin(); it != junc_opts.end(); ++it){
		bool unite_val_exists = false;
		for (unsigned long long i = 0;(!unite_val_exists) && (i < unite_vals.size()); ++i){
			if ((*it)->unite == unite_vals[i]){
				//unite_val_settings[i]->push_back((*it));
				unite_val_exists = true;
			}
		}
		if (!unite_val_exists){
			//unite_val_settings.push_back(new vector<junc_opt*>);
			unite_vals.push_back((*it)->unite);
			//unite_val_settings.back()->push_back(*it);
		}
	}
}
void arrange_unite_vals_all(vector<junc_opt*> junc_opts){
	arrange_unite_vals(junc_opts, junc_opt::unite_vals_all);
}
void arrange_unite_vals_loop(vector<junc_opt*> junc_opts){
	arrange_unite_vals(junc_opts, junc_opt::unite_vals_loop);
}
void save_junc_data(vector<junc_opt*> vec){
	for (auto it = vec.begin(); it != vec.end(); ++it){
		(*it)->save_data();
	}
}
vector<junc_opt*> junc_dist_all;
vector<junc_opt*> junc_dist_loop;

vector<junc_opt*> junc_conn_all;
vector<junc_opt*> junc_conn_loop;

//string junc_dist_all_path = "";
//string junc_dist_loop_path = "";

//string junc_conn_all_path = "";
//string junc_conn_loop_path = "";


string aux_data_path = "";


string scale_unit = "";

string tracing_data_path = "";
string loop_data_path = "";


string animation_path = "";
Scalar animation_color = Scalar(0,0,255);
double animation_thickness = 1.0;



vector<string> split_string_exclude(string s, string t){
	vector<string> res;
	long long spacepos = 0;
	while((spacepos = s.find_first_of(t)) != -1){
		if (spacepos != 0){
			res.push_back(s.substr(0,spacepos));
		}
		s = s.substr(spacepos + 1);
	}
	if (s != ""){
		res.push_back(s);
	}
	return res;
}

vector<string> scale_params(vector<string> w, bool& successful, bool& scaled){
	vector<string> res;
	
	for (unsigned long long i = 0; i < w.size() && successful; ++i){
		if (w[i] != scale_unit){
			res.push_back(w[i]);
		}
		else if (i >= 2) {
			try{
				res.back() = to_string(stod(res.back())/scaling_factor);
			}
			catch(const std::invalid_argument& ia){
				cout << "when trying to convert: \"" << w[i-1] << "\" from " << scale_unit << " to pixel, it could not be interpreted as a number" << endl;
				successful = false;
			}
			if (successful) {
				cout << w[i-1] << " " << w[i] << " was converted to " << res.back() << " pixel" << endl;
				scaled = true;
			}
		}
	}
	return res;
}

bool split_path(string full_path, string& folder, string& file, string& ending){
	bool directory = false;
	long long lastslash = full_path.find_last_of("/");
	long long lastdot = full_path.find_last_of(".");
	
	
	if (lastslash == full_path.length() - 1){
		folder = full_path;
		directory = true;
	}
	else if (lastdot < lastslash && lastdot != -1){
			folder = full_path.substr(0,lastslash + 1);
			file = full_path.substr(lastslash + 1);
	}
	else if(lastdot != -1){
		//cout << "folder: " + full_path.substr(0,lastslash + 1) + "\nfile: " + full_path.substr(lastslash + 1,lastdot) + "\nending: " << full_path.substr(lastdot) << endl;
		folder = full_path.substr(0,lastslash + 1);
		file = full_path.substr(lastslash + 1,lastdot - lastslash - 1);
		ending = full_path.substr(lastdot);
		//cout << "folder: " + full_path.substr(0,lastslash + 1) + "\nfile: " +file + " " + full_path.substr(lastslash + 1,lastslash + 3) << endl;
		//cout << "ending: " << full_path.substr(lastdot,-1) << endl;
	}
	else {
		folder = full_path.substr(0,lastslash + 1);
		file = full_path.substr(lastslash + 1,lastdot);
		//cout << "folder: " + full_path.substr(0,lastslash + 1) + "\nfile: " + full_path.substr(lastslash + 1) << endl;
		//cout << "no dot" << endl;
	}
	return (!directory);
}


/*
int main(){
	vector<string> test = split_string_exclude("   zhis  is  a  test    masafaka"," ");
	for(unsigned long long i = 0; i < test.size(); ++i){
		cout << ":" << test[i] << ":" << endl;
	}
}
*/

bool is_number(string s,double& n){
	try{
		n = stod(s);
		return true;
	}
	catch(const std::invalid_argument& ia){
		return false;
	}
}

bool is_number(string s,long long& n){
	try{
		n = stoll(s);
		return true;
	}
	catch(const std::invalid_argument& ia){
		return false;
	}
}
bool is_number(string s,unsigned long long& n){
	try{
		n = stoull(s);
		return true;
	}
	catch(const std::invalid_argument& ia){
		return false;
	}
}
bool is_number(string s){
	try{
		stod(s);
		return true;
	}
	catch(const std::invalid_argument& ia){
		return false;
	}
}


//bool interprete_save(vecotr<string> w){ //I know that in unix world we use the word "write" instead of "save", but this programm might be used by windows users therefore i use "save" as a keyword.
	//
//}



//draw network 255 0 0 2 only_loops 0 0 255 1 original name .png
bool interprete_draw(vector<string> w){
	bool successful = true;
	unsigned long long l = w.size() - 1;
	draw_commands.push_back(new draw_command);
	for (unsigned long long i = 1; i <= l && successful; ++i){
		if (w[i] == "network"){
			if (!(draw_commands.back()->all_nodes)){
				draw_commands.back()->all_nodes = true;
				if (l >= i + 1 && is_number(w[i+1])){
					if (l >= i + 4 && is_number(w[i+2]) && is_number(w[i+3]) && is_number(w[i+4])){
						int r = stoi(w[i+1]);
						int g = stoi(w[i+2]);
						int b = stoi(w[i+3]);
						if ( 0 <= r && 0 <= g && 0 <= b && 255 >= r && 255 >= g && 255 >= b){
							draw_commands.back()->all_nodes_color = Scalar(b,g,r);
						}
						else {
							cout << "ERROR: color values have to be between 0 and 255" << endl;
							successful = false;
						}
						draw_commands.back()->all_nodes_thickness = abs(stoi(w[i+4]));
						i += 4;
					}
					else if (l >= i + 3 && is_number(w[i+2]) && is_number(w[i+3])){
						int r = stoi(w[i+1]);
						int g = stoi(w[i+2]);
						int b = stoi(w[i+3]);
						if ( 0 <= r && 0 <= g && 0 <= b && 255 >= r && 255 >= g && 255 >= b){
							draw_commands.back()->all_nodes_color = Scalar(b,g,r);
							i += 3;
						}
						else {
							cout << "ERROR: color values have to be between 0 and 255" << endl;
							successful = false;
						}
					}
					else if (l >= i + 2 && is_number(w[i+2])) {
						int c = stoi(w[i+1]);
						if ( 0 <= c && 255 >= c){
							draw_commands.back()->all_nodes_color = Scalar(c,c,c);
						}
						else {
							cout << "ERROR: color values have to be between 0 and 255" << endl;
							successful = false;
						}
						draw_commands.back()->all_nodes_thickness = abs(stoi(w[i+2]));
						i += 2;
					}
					else {
						int c = stoi(w[i+1]);
						if ( 0 <= c && 255 >= c){
							draw_commands.back()->all_nodes_color = Scalar(c,c,c);
							i += 1;
						}
						else {
							cout << "ERROR: color values have to be between 0 and 255" << endl;
							successful = false;
						}
					}
				}
			}
			else {
				cout << "ERROR: the way all node connections are supposed to be drawn was already specified" << endl;
				successful = false;
			}
		}
		else if (w[i] == "only_loops"){
			if(!(draw_commands.back()->only_loops)){
				draw_commands.back()->only_loops = true;
				if (l >= i + 1 && is_number(w[i+1])){
					if (l >= i + 4 && is_number(w[i+2]) && is_number(w[i+3]) && is_number(w[i+4])){
						int r = stoi(w[i+1]);
						int g = stoi(w[i+2]);
						int b = stoi(w[i+3]);
						if ( 0 <= r && 0 <= g && 0 <= b && 255 >= r && 255 >= g && 255 >= b){
							draw_commands.back()->only_loops_color = Scalar(b,g,r);
						}
						else {
							cout << "ERROR: color values have to be between 0 and 255" << endl;
							successful = false;
						}
						draw_commands.back()->only_loops_thickness = abs(stoi(w[i+4]));
						i += 4;
					}
					else if (l >= i + 3 && is_number(w[i+2]) && is_number(w[i+3])){
						int r = stoi(w[i+1]);
						int g = stoi(w[i+2]);
						int b = stoi(w[i+3]);
						if ( 0 <= r && 0 <= g && 0 <= b && 255 >= r && 255 >= g && 255 >= b){
							draw_commands.back()->only_loops_color = Scalar(b,g,r);
							i += 3;
						}
						else {
							cout << "ERROR: color values have to be between 0 and 255" << endl;
							successful = false;
						}
					}
					else if (l >= i + 2 && is_number(w[i+2])) {
						int c = stoi(w[i+1]);
						if ( 0 <= c && 255 >= c){
							draw_commands.back()->only_loops_color = Scalar(c,c,c);
						}
						else {
							cout << "ERROR: color values have to be between 0 and 255" << endl;
							successful = false;
						}
						draw_commands.back()->only_loops_thickness = abs(stoi(w[i+2]));
						i += 2;
					}
					else {
						int c = stoi(w[i+1]);
						if ( 0 <= c && 255 >= c){
							draw_commands.back()->only_loops_color = Scalar(c,c,c);
							i += 1;
						}
						else {
							cout << "ERROR: color values have to be between 0 and 255" << endl;
							successful = false;
						}
					}
				}
			}
			else{
				cout << "ERROR: the way the node connections involved in closed loops are supposed to be drawn was already specified" << endl;
				successful = false;
			}
		}
		else if (w[i] == "original" || w[i] == "cropped" || w[i] == "tubeness" || w[i] == "hessian" || w[i] == "visualized_hessian" || w[i] == "white" || w[i] == "black"){
			if (!(draw_commands.back()->background_specified)){
				if (w[i] != "original"){
					draw_commands.back()->cropped = true;
				}
				draw_commands.back()->background = w[i];
			}
			else {
				cout << "ERROR: image to be drawn on was alreadey specified" << endl;
				successful = false;
			}
		}
		else if (w[i] == "name"){
			if(!(draw_commands.back()->name_specified)){
				++i;
				draw_commands.back()->name_specified = true;
				split_path(w[i], draw_commands.back()->folder, draw_commands.back()->name, draw_commands.back()->ending);
				
				
				if (draw_commands.back()->ending != "" ){
					if (draw_commands.back()->ending == ".bmp" || draw_commands.back()->ending == ".bmp" || draw_commands.back()->ending == ".jpeg" || draw_commands.back()->ending == ".jpg" || draw_commands.back()->ending == ".jpe" || draw_commands.back()->ending == ".jp2" || draw_commands.back()->ending == ".png" || draw_commands.back()->ending == ".pbm" || draw_commands.back()->ending == ".pgm" || draw_commands.back()->ending == ".ppm" || draw_commands.back()->ending == ".sr" || draw_commands.back()->ending == ".ras" || draw_commands.back()->ending == ".tiff" || draw_commands.back()->ending == ".tif" ){
					}
					else{
						cout << "ERROR: file ending:\"" << draw_commands.back()->ending << "\" not supported. Supporded endings are: .bmp, .bmp, jpeg, .jpg, .jpe, .jp2, .png, .pbm, .pgm, .ppm, .sr, .ras, .tiff and .tif" << endl;
						successful = false;
					}
				}
			}
			else {
				cout << "ERROR: name of the image to be saved was already specified" << endl;
				successful = false;
			}
		}
		else {
			successful = false;
			cout << "ERROR: Argument:\"" << w[i] << "\" of drawing command could not be interpreted" << endl;
		}
	}
	
	return successful;
}

bool read_settings_line(string l){
	bool successful = true;
	vector<string> w = split_string_exclude(l," "); // w for words (i know i can't code)
	

	if (w.size() != 0){
		
		cout << endl << "Interpreting: \""+ l + "\"" << endl;

		bool scaled = false;
		if (scale_unit != ""){
			w = scale_params(w, successful, scaled);
		}
		
		
		if (w[0] == "threads"){
			if (!THREADS_set){
				if (w.size() == 2){
					try{
						THREADS = abs(stoi(w[1]));
						cout << THREADS << " threads will be used for the computation" << endl;
					}
					catch( const std::invalid_argument& ia){
						cout << "ERROR: value intended for the number of threads: \"" << w[1] << " could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: To many input arguments for threads. It expects 1 argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else {
				cout << "ERROR: number of threads was already set" << endl;
				successful = false;
			}
		}
		else if (w[0] == "sigma_conv"){
			if (!SG_set && successful){
				if (w.size() == 2){
					try{
						SG = abs(stod(w[1]));
						cout << "sigma_conv set to: " << SG << " pixel" << endl;
						SG_set = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: value intended for sigma_conv: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for sigma_conv, it requires 1 input argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: sigma_conv was already set to: " << SG << " pixel" << endl;
				successful = false;
			}
		}
		else if (w[0] == "r_min"){
			if (!RM_set){
				if (w.size() == 2){
					try{
						RM = abs(stod(w[1]));
						cout << "r_min set to: " << RM << " pixel" << endl;
						RM_set = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: value intended for r_min: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for r_min, it requires 1 input argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: r_min was already set to: " << RM << " pixel" << endl;
				successful = false;
			}
		}
		else if (w[0] == "r_max"){
			if (!RV_set){
				if (w.size() == 2){
					try{
						RV = abs(stod(w[1]));
						cout << "r_max set to: " << RV << " pixel" << endl;
						RV_set = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: value intended for r_max: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for r_max, it requires 1 input argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: r_max was already set to: " << RV << " pixel" << endl;
				successful = false;
			}
		}
		else if (w[0] == "sigma_smooth"){
			if (scaled) {
				successful = false;
				cout << "sigma_smooth has the unit radians and is scale invariat, trying to scale it with the image is senseless" << endl;
			}
			else if (!DEV_set){
				if (w.size() == 2){
					try{
						DEV = abs(stod(w[1]));
						cout << "sigma_smooth set to: " << DEV << " radians" << endl;
						DEV_set = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: value intended for sigma_smooth: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for sigma_smooth, it requires 1 input argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: sigma_smooth was already set to: " << DEV << " radians" << endl;
				successful = false;
			}
		}
		else if (w[0] == "steps"){
			if (scaled) {
				successful = false;
				cout << "steps has no unit and is scale invariat, trying to scale it with the image is senseless" << endl;
			}
			else if (!STEPS_set && successful){
				if (w.size() == 2){
					try{
						STEPS = abs(stoi(w[1]));
						cout << "steps set to: " << STEPS << endl;
						STEPS_set = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: value intended for steps: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for steps, it requires 1 input argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: steps was already set to: " << STEPS << endl;
				successful = false;
			}
		}
		else if (w[0] == "thresh"){
			if (scaled) {
				successful = false;
				cout << "thresh is specified in arbitrary units and is scale invariat, trying to scale it with the image is senseless" << endl;
			}
			if (!TH_set && successful){
				if (w.size() == 2){
					try{
						TH = stod(w[1]);
						cout << "thresh set to: " << TH << endl;
						TH_set = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: value intended for thresh: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for thresh, it requires 1 input argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else if (!scaled){
				cout << "ERROR: thresh was already set to: " << TH << endl;
				successful = false;
			}
		}
		else if (w[0] == "r_step"){
			if (!RS_set){
				if (w.size() == 2){
					try{
						RS = abs(stod(w[1]));
						cout << "r_step set to: " << RS << " pixel" << endl;
						RS_set = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: value intended for r_step: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for r_step, it requires 1 input argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: r_step was already set to: " << RS << " pixel" << endl;
				successful = false;
			}
		}
		else if (w[0] == "min_loop_length"){
			if (scaled) {
				successful = false;
				cout << "thresh is specified in arbitrary units and is scale invariat, trying to scale it with the image is senseless" << endl;
			}
			else if (!ML_set){
				if (w.size() == 2){
					try{
						ML = abs(stoi(w[1]));
						cout << "min_loop_length set to: " << ML << endl;
						ML_set = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: value intended for min_loop_length: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for min_loop_length, it requires 1 input argument instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: min_loop_length was already set to: " << ML << endl;
				successful = false;
			}
		}
		else if (w[0] == "crop"){
			if (!cropped){
				if (w.size() == 5){
					try{
						crop_x1 = abs(stoi(w[1]));
						crop_y1 = abs(stoi(w[2]));
						crop_x2 = abs(stoi(w[3]));
						crop_y2 = abs(stoi(w[4]));
						cout << "The two corners of the cropping rectangle are set to: (x,y) = (" << crop_x1 << "," << crop_y1 << ") and (" << crop_x2 << "," << crop_y2 << ") pixel" << endl;
						cropped = true;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: At least one of the values intended for cropping: \"" << w[1] << " " << w[2] << " " << w[3] << " " << w[4] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: wrong number of input arguments for crop, crop requires 4 input arguments instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: min_loop_length was already set to: " << ML << endl;
				successful = false;
			}
		}
		else if (w[0] == "startpoint"){
			if (w.size() == 3){
				try{
					man_start_points.push_back(new Point(abs(stoi(w[1])),abs(stoi(w[2]))));
					cout << "Start point added at: (x,y) = (" << man_start_points.back()->x << "," << man_start_points.back()->y << ")" << endl;
					start_points_exist = true;
				}
				catch(const std::invalid_argument& ia){
					cout << "ERROR: At least one of the values intended as coordinates for a start point: \"" << w[1] << " " << w[2] << w[4] << "\" could not be interpreted as a number" << endl;
					successful = false;
				}
			}
			else {
				cout << "ERROR: wrong number of input arguments for startpoint, it requires 2 input arguments instead of " << w.size() - 1 << endl;
				successful = false;
			}
		}
		else if (w[0] == "auto_startpoint"){
			if (!auto_start_points){
				if (w.size() == 4){
					try{
						auto_start_points = true;
						n_startpoints = abs(stoi(w[1]));
						rad_startpoints = abs(stoi(w[2]));
						rad_startpoints_specified = true;
						thresh_startpoints = stoi(w[3]);
						cout << "Startpoints will be generated automatically.\nThe maximum number of generated start points is set to:" << n_startpoints << "\n The minimum distance between generated startpoints is set to:" << rad_startpoints << " pixel" << "\n The threshold for startpoints is set to:" << thresh_startpoints << endl;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: At least one of the arguments intended for the automated start point generation: \"" << w[1] << " " << w[2] << " " << w[3] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else if (w.size() == 3){
					try{
						auto_start_points = true;
						n_startpoints = abs(stoi(w[1]));
						rad_startpoints = abs(stoi(w[2]));
						rad_startpoints_specified = true;
						cout << "Startpoints will be generated automatically.\nThe maximum number of generated start points is set to:" << n_startpoints << "\n The minimum distance between generated startpoints is set to:" << rad_startpoints << " pixel" << endl;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: At least one of the arguments intended for the automated start point generation: \"" << w[1] << " " << w[2] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else if (w.size() == 2){
					try{
						auto_start_points = true;
						n_startpoints = abs(stoi(w[1]));
						cout << "Startpoints will be generated automatically.\nThe maximum number of generated start points is set to:" << n_startpoints << endl;
					}
					catch(const std::invalid_argument& ia){
						cout << "ERROR: At least one of the arguments intended for the automated start point generation: \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
				}
				else if (w.size() == 1){
					auto_start_points = true;
					cout << "Startpoints will be generated automatically" << endl;
				}
				else {
					cout << "ERROR: wrong number of input arguments for auto_startpoint, it requires 0 to 3 input arguments instead of " << w.size() - 1 << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: Automated start point generation was already activated" << endl;
				successful = false;
			}
		}
		//draw network 255 0 0 only_loops 0 0 255 original .png
		else if (w[0] == "draw"){
			successful = interprete_draw(w);
		}
		else if (w[0] == "save_loop_areas"){
			loop_area_settings.push_back(new save_loops);
			if (w.size() >= 2){
				if (is_number(w[1])){
					loop_area_settings.back()->thickness = abs(stod(w[1]));
					if (w.size() == 3){
						loop_area_settings.back()->path = w[2];
					}
					else if (w.size() != 2){
						cout << "ERROR: to many arguments for \"save_loop_areas\", no more than 2 arguments are possible instead of "<< w.size() - 1 << endl;
						successful = false;
					}
				}
				else {
					loop_area_settings.back()->path = w[1];
					if (w.size() == 3){
						try{
							loop_area_settings.back()->thickness = abs(stod(w[2]));
						}
						catch(const std::invalid_argument& ia){
							cout << "ERROR: value intended for fiber thickness:\"" << w[2] << "\" could not be interpreted as a number" << endl;
							successful = false;
						}
					}
					else if (w.size() != 2) {
						cout << "ERROR: to many arguments for \"save_loop_areas\", no more than 2 arguments are possible instead of "<< w.size() - 1 << endl;
						successful = false;
					}
				}
			}
			else {
			 loop_area_settings.back()->path = "<imagename>_loop_areas.dat";
			}
			if (successful) {
				cout << "loop areas will be saved to: \"" << loop_area_settings.back()->path << "\" and, for their computation, the fiber diameter is assumed to be " << loop_area_settings.back()->thickness << " pixel " << endl;
			}
		}
		else if (w[0] == "save_loop_circumferences"){
			if (loop_circumference_path == ""){
				if(w.size() == 1){
					loop_circumference_path = "<imagename>_loop_circumferences";
				}
				else if (w.size() == 2){
					loop_circumference_path = w[1];
				}
				else {
					cout << "ERROR: to many arguments for \"save_loop_circumferences\", no more than 1 argument is possible instead of "<< w.size() - 1 << endl;
					successful = false;
				}
				cout << "loop circumferences will be saved to: \"" << loop_circumference_path << "\"" << endl;
			}
			else {
				cout << "ERROR: \"save_loop_circumferences\" was already called" << endl;
				successful = false;
			}
		}
		else if (w[0] == "save_all_junction_distances"){
			if (w.size() <= 3){
				junc_dist_all.push_back(new junc_opt(w,"<imagename>_all_junc_dist.dat",successful));
			}
			else{
				cout << "ERROR: to many arguments for save_all_junction_distances. It accepts no more than 2 arguments " << endl;
			}
		}
		else if (w[0] == "save_loop_junction_distances"){
			if (w.size() <= 3){
				junc_dist_loop.push_back(new junc_opt(w,"<imagename>_loop_junc_dist.dat",successful));
			}
			else{
				cout << "ERROR: to many arguments for save_loop_junction_distance. It accepts no more than 2 arguments " << endl;
			}
		}
		else if (w[0] == "save_all_junction_connectivities"){
			if (w.size() <= 3){
				junc_conn_all.push_back(new junc_opt(w,"<imagename>_all_junc_conn.dat",successful));
			}
			else{
				cout << "ERROR: to many arguments for save_all_junction_connectivitie. It accepts no more than 2 arguments " << endl;
			}
		}
		else if (w[0] == "save_loop_junction_connectivities"){
			if (w.size() <= 3){
				junc_conn_loop.push_back(new junc_opt(w,"<imagename>_loop_junc_conn.dat",successful));
			}
			else{
				cout << "ERROR: to many arguments for save_loop_junction_connectivitie. It accepts no more than 2 arguments " << endl;
			}
		}
		else if (w[0] == "save_auxiliary_data"){
			if (aux_data_path == ""){
				if (w.size() == 2) {
					aux_data_path = w[1];
				}
				else if (w.size() == 1){
					aux_data_path = "<imagename>_auxiliary.dat";
				}
				else {
					cout << "ERROR: to many arguments for save_auxillary_data. It accepts no more than 1 argument " << endl;
					successful = false;
				}
			}
			else{
				cout << "ERROR: \"save_auxiliary_data\" was already called" << endl;
				successful = false;
			}
		}
		else if (w[0] == "set_scale"){
			if (scale_unit == ""){
				unsigned long long equal_pos = 0;
				unsigned long long px_pos = 0;
				
				double px = 1;
				double unit = 1;
				
				for (unsigned long long i = 1;successful && i < w.size(); ++i){
					if (w[i] == "="){
						if (equal_pos == 0){
							equal_pos = i;
						}
						else {
							cout << "ERROR: two \"=\" characters detected in set scale" << endl;
							successful = false;
						}
					}
					else if (w[i] == "px" || w[i] == "pixel"){
						if (px_pos == 0){
							px_pos = i;
						}
						else {
							cout << "ERROR: two \"px\" or \"pixel\" keywords detected in set scale" << endl;
							successful = false;
						}
					}
				}
				//set_scale 300 px = 1 um
				if (px_pos == 2){
					try{
						px = abs(stod(w[1]));
					}
					catch(const std::invalid_argument& ia){
						cout << " value \"" << w[1] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
					
					if (equal_pos == 3 && w.size() == 6 && successful) {
						try{
							unit = abs(stod(w[4]));
						}
						catch(const std::invalid_argument& ia){
							cout << " value \"" << w[4] << "\" could not be interpreted as a number" << endl;
							successful = false;
						}
						if ( !is_number(w[5])){
							scale_unit = w[5];
						}
						else {
							cout << "ERROR: scaling unit must not begin with a digit" << endl;
							successful = false;
						}
					}
				}
				else if (px_pos == 5){
					try{
						px = abs(stod(w[4]));
					}
					catch(const std::invalid_argument& ia){
						cout << " value \"" << w[4] << "\" could not be interpreted as a number" << endl;
						successful = false;
					}
					
					if (equal_pos == 3 && w.size() == 6 && successful) {
						try{
							unit = abs(stod(w[1]));
						}
						catch(const std::invalid_argument& ia){
							cout << " value \"" << w[1] << "\" could not be interpreted as a number" << endl;
							successful = false;
						}
						if ( !is_number(w[2])){
							scale_unit = w[2];
						}
						else {
							cout << "ERROR: scaling unit must not begin with a digit" << endl;
							successful = false;
						}
					}
					else{
						cout << "ERROR: set_scale could not be interpreted" << endl;
						successful = false;
					}
				}
				else {
					cout << "ERROR: set_scale could not be interpreted" << endl;
					successful = false;
				}
				if (successful) {
					scaling_factor = unit/px;
					cout << "Image scale set to " << scaling_factor << " " << scale_unit << " per pixel" << endl;
					cout << "All output data relating to lenghts or areas will be written in the units " << scale_unit <<" or " << scale_unit << "² and scaled accordingly" << endl;
				}
			}
			else {
				cout << "ERROR: scale was already set" << endl;
				successful = false;
			}
		}
		else if (w[0] == "save_tracing_data") {
			if ( tracing_data_path == ""){
				if (w.size() == 2){
					tracing_data_path = w[1];
				}
				else if (w.size() == 1){
					tracing_data_path = "<imagename>_tracing_data.dat";
				}
				else {
					successful = false;
					cout << "to many argumets for save_tracing_data, no more than one argument allowed" << endl;
				}
			}
			else {
				cout << "save_tracing_data was already called" << endl;
			}
		}
		else if (w[0] == "save_loop_data") {
			if ( loop_data_path == ""){
				if (w.size() == 2){
					loop_data_path = w[1];
				}
				else if (w.size() == 1){
					loop_data_path = "<imagename>_loop_data.dat";
				}
				else {
					successful = false;
					cout << "to many argumets for save_loop_data, no more than one argument allowed" << endl;
				}
			}
			else {
				cout << "save_loop_data was already called" << endl;
			}
		}
		else if (w[0] == "animate_tracing"){
			if (animation_path == ""){
				animation_path = "<imagename>_animated.avi";
				if (w.size() == 2){
					try{
						animation_thickness = stod(w[1]);
					}
					catch(const std::invalid_argument& ia){
						cout << w[1] << " could not be interpreted as a number" << endl;
						successful = false;
					}
				}
			}
			else {
				cout << "animate_tracing was already called" << endl;
			}
		}
		else if (w[0] == "use_modified_hessian"){
			modified_hessian = true;
		}
		else if (w[0] == "delete_nodes_below_threshold"){
			if (w.size() == 2){
				if (!is_number(w[1],node_thresh_below)){
					cout << "threshold can only be a number" << endl;
					successful = false;
				}
				else {
					del_nodes_below_thresh = true;
				}
			}
			else{ successful = false; cout << w[0] << " can only take exactly one argument" << endl;}
		}
		else if (w[0] == "delete_nodes_above_threshold"){
			if (w.size() == 2){
				if (!is_number(w[1],node_thresh_above)){
					cout << "threshold can only be a number" << endl;
					successful = false;
				}
				else {
					del_nodes_above_thresh = true;
				}
			}
			else{ successful = false; cout << w[0] << " can only take exactly one argument" << endl;}
		}
		else if (w[0] == "delete_lines_below_threshold"){
			if (w.size() == 2){
				if (!is_number(w[1],node_line_thresh_below)){
					cout << "threshold can only be a number" << endl;
					successful = false;
				}
				else {
					del_lines_below_thresh = true;
				}
			}
			else{ successful = false; cout << w[0] << " can only take exactly one argument" << endl;}
		}
		else if (w[0] == "delete_lines_above_threshold"){
			if (w.size() == 2){
				if (!is_number(w[1],node_line_thresh_above)){
					cout << "threshold can only be a number" << endl;
					successful = false;
				}
				else {
					del_lines_above_thresh = true;
				}
			}
			else{ successful = false; cout << w[0] << " can only take exactly one argument" << endl;}
		}
		else if (w[0] == "save_loop_line_lengths" || w[0] == "save_all_line_lengths"){
			line_analysis::line_options.push_back(new line_analysis(w,successful));
		}
		
		//string animation_path = "";
		//Scalar animation_color = Scalar(0,0,255);
		//double animation_thickness = 1.0;
		
		
		else {
			cout << "ERROR: settings line:\"" + l + "\" could not be interpreted" << endl;
			successful = false;
		}
	}
	return successful;
}


bool read_settings(char* filename){
	bool successful = true;
	string l; //line
	ifstream f(filename); //file
	if (f.is_open()){
		while (getline(f,l) && successful){
			l = l.substr(0,l.find_first_of("#"));
			successful = read_settings_line(l);
		}
		f.close();
	}
	else{
		cout << "ERROR: settings file not found" << endl;
		successful = false;
	}
	return successful;
}


bool file_specified = false;


string folder = "";
string file = "";
string ending = "";


string replace_keywords(string s){
	string ret = s;
	long long pos = 0;
	string find = "<imagename>";
	string replace = file;
	pos = ret.find(find);
	while (pos != -1){
		ret.replace(pos,find.size(),replace);
		pos = ret.find(find);
	}
	return ret;
}

unsigned long long find_node_list_position(node*, vector<node*>);
void save_tracing_data(vector<node*> list, string path){
	ofstream f;
	f.open(path);
	if (f.is_open()){
		for (unsigned long long i = 0; i < list.size(); i++){
			f << i << " " << list[i]->x*scaling_factor << " " << list[i]->y*scaling_factor << endl;
		}
		
		for (unsigned long long i = 0; i < list.size(); ++i){
			for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
				f << i << " " << find_node_list_position(list[i]->connections[j],list) << endl;
			}
		}
	}
	else {
		cout << "WARNING: Path to save tracing data: \"" << path << "\" could not be opened" << endl;
	}
	f.close();
}

void save_loop_data(vector<vector<node*>> loops, string path){
	
	ofstream f;
	f.open(path);
	
	if (f.is_open()){
		for (unsigned long long i = 0; i < loops.size() ; ++i){
			for (unsigned long long j = 0; j < (loops[i]).size(); ++j){
				f << i << " " << loops[i][j]->x*scaling_factor << " " << loops[i][j]->y*scaling_factor << endl;
			}
		}
	}
	else {
		cout << "WARNING: Path to save tracing data: \"" << path << "\" could not be opened" << endl;
	}
	f.close();
}

void save_aux_data(vector<double> data, vector<string> labels, string path){
	ofstream f;
	f.open(path);

	if (f.is_open()){
		for (auto i = 0;i < data.size(); ++i){
			f << labels[i] << ": " << data[i] << endl;
		}
	}
}

string str_add_double(string str, unsigned long long d){
	ostringstream cvt;
	cvt << str << d;
	return cvt.str();
}
/*
int main(){
	Mat tmp(50,50, CV_8UC3, Scalar::all(255));
	if (imwrite("./test/test.png", tmp)){
		cout << "success" << endl;
	}
	else {
		cout << "faliour" << endl;
	}
}
*/
#endif
