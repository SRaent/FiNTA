
#define PRINT(x) cout << #x << " => " << x << endl;

#define SQRT2 (double)1.4142135623730950488016

#define PX 412
#define PY 495
#define RV 10 //vision radius
#define RS 3 //step radius
#define RT RV// vision for threshold
#define RN RF * 2 //neighbor radius
#define RF RS // SQRT2  //forbidden radius
#define RM 0 //minimum vision radius
#define STEPS 100
#define DEV 0.63 //deviation of gaussian smooth of circlefun
#define LT 2 // line thiccness for connectable test
#define LD 0.2 //deviation of smoothing if line function for connectable test
#define LS RS*LT // steps for averaging the line function
#define CT 3 // Connectabel threshhold. if the smoothed function goes below this, no new node will be spawned.
#define ML 7 //minimum loop length


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




//images are saved in 2 dimensional arrays and the pixels are to be adressed by img[x][y] this has the effect, that any loops acessing all pixels have to loop x first and then y so y is the faster changin index(for performance reasons). but when actually writing image data to a file, doing this will result in x and y being swapped, unless the loop order is swiched(sacreficing some performance).



int main(){
	Mat I2 = imread("/home/moritz/Documents/Moritz_pics/Manu_Data/Pictures/M_Blebb_c3_Perifery.tif", CV_LOAD_IMAGE_GRAYSCALE);
	if(!I2.data){
		cout << "bild existiert NICHT" << endl;
		return -1;
	}
	Size s = I2.size();
	Rect myROI(0,0,1024,884);
	Mat I3 = I2(myROI);
	Mat I5;
	I3.convertTo(I5, CV_64F);

//	fftw_complex* FI1 = fft_2d(I3);
	
//	cutouteroval_ft(FI1,1.6/8,s.width,s.height);
//	cutinneroval_ft(FI1,0.4/8,s.width,s.height);
//	cutinneroval_ft(FI1,0.035355,s.width,s.height);
	
//	Mat I4 = cv_ifft_2d_real(FI1,s.width,s.height);
	
	
//	normalize(I5,I5,1,0,32);
//	Scalar meanI5 = mean(I5);
//	clamp(I5,meanI5[0] * 0.85,1);
	
	normalize(I5,I5,255,0,32);
	/*
	Mat kernel;
	double delta = 0;
	int kernel_size = 7;
	Point anchor(kernel_size/2,kernel_size/2);
	kernel = Mat::ones(kernel_size, kernel_size, CV_64F)/(float)(kernel_size * kernel_size);
	
	filter2D(I3, I5, -1 ,kernel, anchor, 0, BORDER_DEFAULT);
	*/
	
//	circle(I5,Point(s.width / 2,s.height),500,0,10);
//	circle(I5,Point(s.width / 2,s.height),500,255,4);
	
	
	Mat hessian = convolve_hessian(I5,15,3);
	Mat tubeness = tubeness_hessian(hessian);
	Mat viz = visualize_hessian(hessian);
	hessian_weighted_angle_distribution(hessian,10);
//	hessian_weighted_relative_angle_distribution(hessian, 10, 100);
	normalize(tubeness,tubeness,255,0,32);
	normalize(viz,viz,255,0,32);
	/*
	
	vector<node*> list;
	new node(PX,PY,&list,&hessian);
	
	unsigned long long i = 1;
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
	
	for (unsigned long long i = 0; i < list.size(); ++i){
		for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
			line(tubeness,Point(list[i]->x,list[i]->y),Point(list[i]->connections[j]->x,list[i]->connections[j]->y),255);
			
		}
	}
//	line(I5,Point(PX,PY),Point(499.64,413.84),255);
//	line(I5,Point(PX,PY),Point(2,2),255);
//	I2.convertTo(I2,CV_8U);
	hessian.convertTo(hessian, CV_8UC3);*/
	imwrite("doubt.tif",I3);
	imwrite("doubt2.tif",tubeness);
//	imwrite("viz.tif",viz);
	return 0;
}
