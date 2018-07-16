
#define PRINT(x) std::cout << #x << " => " << x << std::endl;

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


using namespace std;


//images are saved in 2 dimensional arrays and the pixels are to be adressed by img[x][y] this has the effect, that any loops acessing all pixels have to loop x first and then y so y is the faster changin index(for performance reasons). but when actually writing image data to a file, doing this will result in x and y being swapped, unless the loop order is swiched(sacreficing some performance).




uint8** I1;

double** I2;

//uint32 w,h;




int main(){
	
//	if(importsemtif("../Franzi_CPD_012.tif",I1,&w,&h)){
//		cout << "image could not be imported" << endl;
//	}
	
//	fftw_complex* FI1 = fft_2d(I1,w,h);
	
	
	
//	I2 = ifft_2d_real(FI1,w,h);
//	cv::Mat I3(h,w, CV_8UC1, cv::Scalar(0));
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
	
	cv::imwrite("Filter1.tif",I5);
	
	
	
	
//	I3.at<uchar>(2,2) = 12;
//	cv::Scalar s = I3.at<uchar>(2,2);
//	I3.data[5] = 255;
//	cv::Scalar s = (I3.at<uchar>(0,5));
//	PRINT(s.val[0])
//	cv::imwrite("gausssmooth.tif", I3);
	
//	cv::Mat cvFT = cv::Mat(s.height,s.width,CV_64FC2);
	
//	cv::dft(I3,cvFT,16);
	
	
/*
	for (unsigned long long i = 0; i < s.height; ++i){
		for (unsigned long long j = 0; j < s.width; ++j){  
			cout << (double)I4.at<double>(i,j) << " ";
		}
		cout << endl;
	}
*/
/*
	for (unsigned long long x = 0; x < s.width; ++x){
		for (unsigned long long y = 0; y < s.height; ++y){
			cout << log(sqrt(pow(FI1[(x * s.height) + y][0],2) + pow(FI1[(x * s.height) + y][1],2)) + 1 ) << " ";
		}
		cout << endl;
	}
*/	
	
}

