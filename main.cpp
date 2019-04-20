
#define PRINT(x) cout << #x << " => " << x << endl;

#define SQRT2 (double)1.4142135623730950488016



#define PX 396 //412
#define PY 238 //495



//#define THREADS 16

unsigned long long THREADS = 16;

//#define RV 8 // 8 vision radius (~2 times fibre thickness)
//#define RM 0 //minimum vision radius
//#define RS 3 //step radius
//#define STEPS 360
//#define DEV 0.5 // 0.55 deviation of gaussian smooth of circlefun
//#define TH 2 //0.5 threshold for findpks
//#define ML 7 //minimum loop length
//#define SG 1.8 //1.8 ; 2.3 (a bit less than half fibre thickness)

double RV = 8;						// 8 vision radius (~2 times fibre thickness)
double RM = 0;						//minimum vision radius
double RS = 3;						//step radius (~ 0.5 - 1 x fiber thickness)
unsigned long long STEPS = 360;		//number of steps the smoothing function is computed for
double DEV = 0.5;					// 0.55 deviation of gaussian smooth of circlefun
double TH = 2;						//0.5 threshold for findpks
unsigned long long ML = 7;			//minimum loop length
double SG = 1.8;					//1.8 ; 2.3 (a bit less than half fibre thickness)



bool RV_set = false;
bool RM_set = false;
bool RS_set = false;
bool STEPS_set = false;
bool DEV_set = false;
bool TH_set = false;
bool ML_set = false;
bool SG_set = false;



bool cropped = false;
double crop_x1 = 0;
double crop_y1 = 0;
double crop_x2 = 0;
double crop_y2 = 0;




//computed
#define RN RF+RS //neighbor radius
#define RF RS/SQRT2  //forbidden radius


//nolonger used
#define RT RV// vision for threshold
#define LT 2 // line thiccness for connectable test
#define LD 0.2 //deviation of smoothing if line function for connectable test
#define LS RS*LT // steps for averaging the line function
#define CT 3 // Connectabel threshhold. if the smoothed function goes below this, no new node will be spawned.
#define FA 0.125*PI //forbidden angle for already connected nodes to spawn new ones


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <complex.h>
#include <fftw3.h>
#include <thread>

#include "tiffio.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

#include "datafun.h"
#include "datafun.cpp"
#include "mathfun.cpp"
#include "node.h"
#include "node.cpp"
#include "analyse.cpp"
#include "visualise.cpp"
#include "generate.cpp"



bool start_points_exist = false;
vector<Point*> man_start_points;

bool auto_start_points = false;
unsigned long long n_startpoints = 100;
double rad_startpoints = 20*RS;
bool rad_startpoints_specified = false;
double thresh_startpoints = 0;


class draw_command{
	public:
	bool all_nodes = false;
	bool only_loops = false;
	Scalar all_nodes_color = Scalar(255,0,0);
	Scalar only_loops_color = Scalar(0,0,255);
	double all_nodes_thickness = 1;
	double only_loops_thickness = 1;
	
	string background = "original";
	
	string name = "";
	
	Mat image;
	
	bool cropped = false;
	bool ending_specified = false;
};

vector<draw_command*> draw_commands;

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
/*
int main(){
	vector<string> test = split_string_exclude("   zhis  is  a  test    masafaka"," ");
	for(unsigned long long i = 0; i < test.size(); ++i){
		cout << ":" << test[i] << ":" << endl;
	}
}
*/

bool is_number(string s){
	return (s[0] == '0' || s[0] == '1' || s[0] == '2' || s[0] == '3' || s[0] == '4' || s[0] == '5' || s[0] == '6' || s[0] == '7' || s[0] == '8' || s[0] == '9');
}
//draw network 255 0 0 only_loops 0 0 255 on original as .png
bool interprete_draw(vector<string> w){
	bool successful = true;
	unsigned long long l = w.size();
	draw_commands.push_back(new draw_command);
	for (unsigned long long i = 1; i < l && successful; ++i){
		if (w[i] == "network"){
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
				if (l >= i + 3 && is_number(w[i+2]) && is_number(w[i+3])){
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
						i += 1;
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
					}
					else {
						cout << "ERROR: color values have to be between 0 and 255" << endl;
						successful = false;
					}
				}
			}
		}
		else if (w[i] == "only_loops"){
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
				if (l >= i + 3 && is_number(w[i+2]) && is_number(w[i+3])){
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
						i += 1;
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
		
		if (w[0] == "sigma_conv"){
			if (!SG_set){
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
			if (!DEV_set){
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
			if (!STEPS_set){
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
			if (!TH_set){
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
			else{
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
			if (!ML_set){
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
		//draw network 255 0 0 only_loops 0 0 255 on original .png
		else if (w[0] == "draw"){
			successful = interprete_draw(w);
		}
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

int main(int n, char** args){
	
	for(int i = 0; i < n; ++i){
		cout << args[i] << " " << i << endl;
		if(strcmp(args[i],"-s") == 0 && ++i < n){
			read_settings(args[i]);
		}
		else if(strcmp(args[i],"-f") == 0 && ++i < n){
			string full_path(args[i]);
			long long lastslash = full_path.find_last_of("/");
			long long lastdot = full_path.find_last_of(".");
			//PRINT(lastslash)
			//PRINT(full_path.length())
			//PRINT(lastdot)
			if (lastslash == full_path.length() - 1){
				cout << "ERROR: Path to image: \"" + full_path + "\" appers to be a directory" << endl;
				return -1;
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
			//cout << "\nfolder: " + folder + "\nfile: " + file + "\nending: " + ending << endl;
		}
	}
	return 0;
}



int main2(){
	
	//Mat I2 = imread("/home/moritz/Documents/Moritz_pics_lap/Franzi_CPD_012.tif");
	folder = "/home/moritz/Documents/Moritz_pics_lap/analysable images/";
//	string file = "10T_2GA_2PFA_RPE1_wt_susp_011";
//	string file = "10T_2GA_2PFA_RPE1_wt_susp_024";
//	string file = "Cx_hmds_4-37_058";
//	string file = "C2_p_hmds_048";
//	string file = "C1_p_hmds_045";
//	string file = "C1_p_hmds_044";
//	string file = "C1_p_LatA_hmds_041";

//	string file = "1T_002";
//	string file = "1T_004";
//	string file = "1T_005";
//	string file = "Blebbistatin_20muM_0.5h_010";
//	string file = "Blebbistatin_20muM_0.5h_011";
//	string file = "Blebbistatin_20muM_0.5h_012";
//	string file = "LatA_adh_100nM_0.25h_005";
//	string file = "LatA_adh_100nM_0.25h_006";
//	string file = "01_CalycA_RPE1_wt_adh_2GA_2PFA027_001";
//	string file = "02_CalycA_RPE1_wt_adh_2GA_2PFA027_002";
	file = "03_CalycA_RPE1_wt_adh_2GA_2PFA027_004";

	string write_folder = "./results/";
	Mat I2 = imread(folder+file+".tif");
	if(!I2.data){
		cout << "ERROR: Image could not be importet" << endl;
		return -1;
	}
	//Mat I2(img);
	Rect myROI(0,0,1024,884);
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
