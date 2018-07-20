
#define PRINT(x) std::cout << #x << " => " << x << std::endl;

#define SQRT2 (double)1.4142135623730950488016

#define PX 412
#define PY 495
#define RV 7 //vision radius
#define RS 5 //step radius
#define RN RS*SQRT2 //neighbor radius
#define RF RS/SQRT2 //forbidden radius
#define STEPS 100
#define DEV 0.4

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <complex.h>
#include <fftw3.h>

#include "tiffio.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


#include "datafun.h"
#include "datafun.cpp"
#include "mathfun.cpp"
#include "node.h"
#include "node.cpp"

using namespace std;



//images are saved in 2 dimensional arrays and the pixels are to be adressed by img[x][y] this has the effect, that any loops acessing all pixels have to loop x first and then y so y is the faster changin index(for performance reasons). but when actually writing image data to a file, doing this will result in x and y being swapped, unless the loop order is swiched(sacreficing some performance).


int main(){
	cv::Mat I2 = cv::imread("../Franzi_CPD_012.tif", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Rect myROI(0,0,1024,884);
	cv::Mat I3 = I2(myROI);
	cv::Size s = I3.size();
	
	fftw_complex* FI1 = fft_2d(I3);
	
	cutouteroval_ft(FI1,0.3,s.width,s.height);
	cutinneroval_ft(FI1,0.00125,s.width,s.height);
	
	cv::Mat I4 = cv_ifft_2d_real(FI1,s.width,s.height);
	
	
	cv::Mat I5(s.height,s.width,CV_64F);
	GaussianBlur(I4,I5,cv::Size(7,7),1);
	
	cv::normalize(I5,I5,1,0,32);
	cv::Scalar meanI5 = mean(I5);
	clamp(I5,meanI5[0] * 0.85,1);
	
	cv::normalize(I5,I5,255,0,32);
	
	
	std::vector<node*> list;
	list.push_back(new node(PX,PY));
	
	(list[0])->list = &list;
	
	for (unsigned long long i = 0; i < 20; ++i){
		unsigned long long end = list.size();
		for (unsigned long long it = 0; it < end; ++it){
			if (!(list[it]->procreated)){
				list[it]->procreate(I5);
			}
		}
		cout << i << endl;
	}
	
	for (unsigned long long i = 0; i < list.size(); ++i){
		for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
			cv::line(I5,cv::Point(list[i]->x,list[i]->y),cv::Point(list[i]->connections[j]->x,list[i]->connections[j]->y),255);
			
		}
	}
//	cv::line(I5,cv::Point(PX,PY),cv::Point(499.64,413.84),255);
//	cv::line(I5,cv::Point(PX,PY),cv::Point(2,2),255);
	cv::imwrite("doubt.tif",I5);
	
	return 0;
}
