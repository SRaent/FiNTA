
#define PRINT(x) cout << #x << " => " << x << endl;

#define SQRT2 (double)1.4142135623730950488016

#define PX 412
#define PY 495
#define RV 8 // 8 vision radius
#define RS 3 //step radius
#define RT RV// vision for threshold
#define RN RF+RS //neighbor radius
#define RF RS/SQRT2  //forbidden radius
#define RM 0 //minimum vision radius
#define STEPS 100
#define DEV 0.55 // 0.55 deviation of gaussian smooth of circlefun
#define LT 2 // line thiccness for connectable test
#define LD 0.2 //deviation of smoothing if line function for connectable test
#define LS RS*LT // steps for averaging the line function
#define CT 3 // Connectabel threshhold. if the smoothed function goes below this, no new node will be spawned.
#define ML 7 //minimum loop length
#define TH 2.6 //2.3 threshold for findpks


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <complex.h>
#include <fftw3.h>

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






int main(){
	Mat I2 = imread("/home/moritz/Documents/Moritz_pics/Franzi_CPD_012.tif", CV_LOAD_IMAGE_GRAYSCALE);
	if(!I2.data){
		cout << "bild existiert NICHT" << endl;
		return -1;
	}
	Size s = I2.size();
	Rect myROI(0,0,1024,884);
	Mat I3 = I2(myROI);
	Mat I5;
	I3.convertTo(I5, CV_64F);
	
	normalize(I5,I5,255,0,32);
	
	
	
	
	Mat hessian = convolve_hessian(I5,15,1.8);
	Mat tubeness = tubeness_hessian(hessian);
	
	
	normalize(tubeness,tubeness,255,0,32);
	
	
	vector<node*> list;
	vector<node**> closures;
	node* origin = new node(PX,PY,&list,&closures,&hessian);
	
	unsigned long long i = 0;
	
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
		cout << i++ << endl;
		
	}
	
	
	//vector<double> angles = con_angles(list);
	//double_vector_to_file("angles.dat",angles);
	
	I3.convertTo(I3, CV_8U);
	cv::cvtColor(I3, I3, cv::COLOR_GRAY2BGR);
	
	I3 = draw_list(I3,list);
	PRINT(list.size())
	only_loops(list);
	
	vector<node*> junctions = find_junctions(list);
	
	I3 = draw_list(I3,list,Scalar(255,0,0));
	
	I3 = draw_list(I3,junctions,Scalar(0,0,0));
	
	//vector<double> angles = con_angles(junctions);
	//double_vector_to_file("angles.dat",angles);
	//vector<vector<node*>> loops = find_loops(closures);
	
	//vector<double> areas = find_loop_areas(loops);
	//double_vector_to_file("areas.dat",areas);
	PRINT(list.size())
	//PRINT(loops.size())
	//PRINT(closures.size())
	/*
	for (int i = 0; i < loops.size(); ++i){
		for (int j = 0; j < loops[i].size(); ++j){
			cout << loops[i][j] << " " ;
		}
		cout << endl;
	}*/
	I3 = draw_closures(I3,closures,Scalar(128,0,128));
	
	//draw_loops("./loops/", loops, I3, true);
	//cout << "total area: " << total_loop_area(loops) << " max area: " << max_loop_area(loops) << endl;
	
	
	imwrite("doubt.tif",I3);
	imwrite("doubt2.tif",tubeness);
	return 0;
}
