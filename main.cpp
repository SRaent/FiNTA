
#define PRINT(x) cout << #x << " => " << x << endl;

#define SQRT2 (double)1.4142135623730950488016

#define PX 412
#define PY 495
#define RV 6 //vision radius
#define RS 3 //step radius
#define RT RV// vision for threshold
#define RN RF * 2 //neighbor radius
#define RF RS / SQRT2  //forbidden radius
#define RM 0 //minimum vision radius
#define STEPS 100
#define DEV 0.55 //deviation of gaussian smooth of circlefun
#define LT 2 // line thiccness for connectable test
#define LD 0.2 //deviation of smoothing if line function for connectable test
#define LS RS*LT // steps for averaging the line function
#define CT 3 // Connectabel threshhold. if the smoothed function goes below this, no new node will be spawned.
#define ML 7 //minimum loop length
#define TH 2.0 // threshold for findpks


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
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




//images are saved in 2 dimensional arrays and the pixels are to be adressed by img[x][y] this has the effect, that any loops acessing all pixels have to loop x first and then y so y is the faster changin index(for performance reasons). but when actually writing image data to a file, doing this will result in x and y being swapped, unless the loop order is swiched(sacreficing some performance).



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
//	Mat viz = visualize_hessian(hessian);
//	hessian_weighted_angle_distribution(hessian,10);
//	hessian_weighted_relative_angle_distribution(hessian, 10, 100);
	normalize(tubeness,tubeness,255,0,32);
//	normalize(viz,viz,255,0,32);
//	/*
	
	vector<node*> list;
	vector<node**> closures;
	new node(PX,PY,&list,&closures,&hessian);
	
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
		cout << i++ << endl;
		
	}
	
	tubeness.convertTo(tubeness, CV_8U);
	cv::cvtColor(tubeness, tubeness, cv::COLOR_GRAY2BGR);
	
	for (unsigned long long i = 0; i < list.size(); ++i){
		for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
			line(tubeness,Point(list[i]->x,list[i]->y),Point(list[i]->connections[j]->x,list[i]->connections[j]->y),Scalar(0, 255, 0));
			
		}
	}
	
	only_loops(list);
	
	
	for (unsigned long long i = 0; i < list.size(); ++i){
		for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
			line(tubeness,Point(list[i]->x,list[i]->y),Point(list[i]->connections[j]->x,list[i]->connections[j]->y),Scalar(255, 0, 0));
			
		}
	}
	
	
	vector<vector<node*>> loops = find_loops(closures);
	
	PRINT(loops.size())
	PRINT(closures.size())/*
	for  (unsigned long long i = 0; i < loops.size(); ++i){
		PRINT(loop_checksum(loops[i]))
	}*/
	
	//vector<node*> loop = find_loop(closures[1],-1);
	
	double total_area = 0;
	double max_area = 0;
	
	Mat loops_img;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		tubeness.copyTo(loops_img);
//		PRINT(loop_area(loops[l]))
		char area[100];
		sprintf(area,"area = %f; length = %f",loop_area(loops[l]),loop_length(loops[l]));
		putText(loops_img,area, cvPoint(20,20), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,255), 1, CV_AA);
		
		total_area += loop_area(loops[l]);
		max_area = max(max_area, loop_area(loops[l]));
		
		for (unsigned long long i = 0; i < loops[l].size() - 1; ++i){
			for (unsigned long long j = 0; j < loops[l][i]->connections.size(); ++j){
				line(loops_img,Point(loops[l][i]->x,loops[l][i]->y),Point(loops[l][i + 1]->x,loops[l][i + 1]->y),Scalar(255, 255 * i / loops[l].size(), 255));
				
			}
		}
		char filename[50];
		sprintf(filename,"./loops/loop%d.tif",l);
		imwrite(filename,loops_img);
	}
	
	cout << "total area: " << total_area/2.0 << " max area: " << max_area << endl;
	
	/*
	for (unsigned long long i = 0; i < loop.size() - 1; ++i){
		for (unsigned long long j = 0; j < loop[i]->connections.size(); ++j){
			line(tubeness,Point(loop[i]->x,loop[i]->y),Point(loop[i + 1]->x,loop[i + 1]->y),Scalar(255, 255 * i / loop.size(), 255));
			
		}
	}*/
	
	for (unsigned long long i = 0; i < closures.size(); ++i){
		line(tubeness,Point(closures[i][0]->x,closures[i][0]->y),Point(closures[i][1]->x,closures[i][1]->y),Scalar(0,0,255));
	}
	
//	hessian.convertTo(hessian, CV_8UC3);//*/
	imwrite("doubt.tif",I3);
	imwrite("doubt2.tif",tubeness);
//	imwrite("viz.tif",viz);
	return 0;
}
