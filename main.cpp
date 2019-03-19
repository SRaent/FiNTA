
#define PRINT(x) cout << #x << " => " << x << endl;

#define SQRT2 (double)1.4142135623730950488016

#define THREADS 16

#define PX 396 //412
#define PY 238 //495

#define RV 8 // 8 vision radius (~2 times fibre thickness)
#define RM 0 //minimum vision radius
#define RS 3 //step radius
#define STEPS 360
#define DEV 0.5 // 0.55 deviation of gaussian smooth of circlefun
#define TH 2 //0.5 threshold for findpks
#define ML 7 //minimum loop length
#define SG 1.8 //1.8 ; 2.3 (a bit less than half fibre thickness)

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




int main(){
	
	//Mat I2 = imread("/home/moritz/Documents/Moritz_pics_lap/Franzi_CPD_012.tif");
	string folder = "/home/moritz/Documents/Moritz_pics_lap/analysable images/";
//	string file = "10T_2GA_2PFA_RPE1_wt_susp_011";
//	string file = "10T_2GA_2PFA_RPE1_wt_susp_024";
	string file = "Cx_hmds_4-37_058";
//	string file = "C2_p_hmds_048";
//	string file = "C1_p_hmds_045";
//	string file = "C1_p_hmds_044";
//	string file = "C1_p_LatA_hmds_041";
	string write_folder = "./results/";
	Mat I2 = imread(folder+file+".tif");
	if(!I2.data){
		cout << "bild existiert NICHT" << endl;
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

	
	
	
	vector<node*> junctions = find_junctions(list);
	cout<< "analysing Junctions" << endl;
	vector<double> jd =  junction_distances(junctions);
	
	double_vector_to_file(write_folder+file+"_junction_dist.dat",jd);
	
	cout<< "done analysing Junctions" << endl;
	
	I3.convertTo(I3, CV_8U);
	cv::cvtColor(I3, I3, cv::COLOR_GRAY2BGR);
	int line_thickness = 4;
	I3 = draw_list(I3,list,Scalar(255,0,0),line_thickness);
	
	PRINT(list.size())
	
	only_loops(list, closures);
	
	I3 = draw_list(I3,list,Scalar(0,0,255),line_thickness);
	
	PRINT(list.size())
	
//	I3 = draw_list(I3,junctions,Scalar(0,0,0));
	
	
	vector<vector<node*>> loops = find_loops(closures);
	
	vector<double> areas = find_loop_areas_wo_max_w_diam(loops,4.0);
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
