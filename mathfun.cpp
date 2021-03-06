#ifndef MATHFUN_CPP

#define PRINT(x) cout << #x << " => " << x << endl;

#define SQRT2 (double)1.4142135623730950488016
#define PI (double)3.1415926535897932384626433832795

#define MATHFUN_CPP MATHFUN_CPP

#include <string>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
//#include <cstdint>

//#include <fftw3.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "userinterface.cpp"

using namespace std;
using namespace cv;



double sqr(double a){
	return a*a;
}

Mat convolve_hessian(Mat img, unsigned long long ksize, double dev){
	Mat kernel[3];
	kernel[0] = Mat(ksize,ksize, CV_64F);
	kernel[1] = Mat(ksize,ksize, CV_64F);
	kernel[2] = Mat(ksize,ksize, CV_64F);
	Point m(ksize/2,ksize/2);
	//PRINT(img.channels())
	Vec3d vals;
	for( unsigned long long y = 0; y < ksize; ++y){
		for (unsigned long long x = 0; x < ksize; ++x){
			//checked for scaling, of by a factor of dev from the true second order derivative
			(kernel[0]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)x - m.x)/dev,2) - 1.0)/(pow(dev,2)*sqrt(PI*2.0));
			(kernel[1]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev)) * ((double)x - m.x) * ((double)y - m.y)/(pow(dev,4) * sqrt(PI*2.0));
			(kernel[2]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)y - m.y)/dev,2) - 1.0)/(pow(dev,2)*sqrt(PI*2.0));
		}
	}
	vector<Mat> res;
	Mat R1;
	Mat R2;
	Mat R3;
	res.push_back(R1);
	res.push_back(R2);
	res.push_back(R3);
	
	filter2D(img, res[0], -1 ,kernel[0], m, 0, BORDER_DEFAULT);
	filter2D(img, res[1], -1 ,kernel[1], m, 0, BORDER_DEFAULT);
	filter2D(img, res[2], -1 ,kernel[2], m, 0, BORDER_DEFAULT);
	//PRINT(res[0].channels())
	Mat ret;
	cv::merge(res,ret);
	return ret;
}

Mat convolve_hessian_plus_derivative(Mat img, unsigned long long ksize, double dev){
	Mat kernel[5];
	kernel[0] = Mat(ksize,ksize, CV_64F);
	kernel[1] = Mat(ksize,ksize, CV_64F);
	kernel[2] = Mat(ksize,ksize, CV_64F);
	kernel[3] = Mat(ksize,ksize, CV_64F);
	kernel[4] = Mat(ksize,ksize, CV_64F);
	Point m(ksize/2,ksize/2);
	
	Vec3d vals;
	for( unsigned long long y = 0; y < ksize; ++y){
		for (unsigned long long x = 0; x < ksize; ++x){
			(kernel[0]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)x - m.x)/dev,2) - 1.0)/(pow(dev,3)*sqrt(PI*2.0));
			(kernel[1]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev)) * ((double)x - m.x) * ((double)y - m.y)/(pow(dev,5) * sqrt(PI*2.0));
			(kernel[2]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)y - m.y)/dev,2) - 1.0)/(pow(dev,3)*sqrt(PI*2.0));
			(kernel[3]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*((double)x - m.x)/(pow(dev,3)*sqrt(PI*2.0));
			(kernel[4]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*((double)y - m.y)/(pow(dev,3)*sqrt(PI*2.0));
		}
	}
	vector<Mat> res;
	Mat R1;
	Mat R2;
	Mat R3;
	Mat gx;
	Mat gy;
	res.push_back(R1);
	res.push_back(R2);
	res.push_back(R3);
	
	filter2D(img, res[0], -1 ,kernel[0], m, 0, BORDER_DEFAULT);
	filter2D(img, res[1], -1 ,kernel[1], m, 0, BORDER_DEFAULT);
	filter2D(img, res[2], -1 ,kernel[2], m, 0, BORDER_DEFAULT);
	filter2D(img, gx, -1 ,kernel[3], m, 0, BORDER_DEFAULT);
	filter2D(img, gy, -1 ,kernel[4], m, 0, BORDER_DEFAULT);
	
	Size s = res[0].size();
	
	for(unsigned long long y = 0; y < s.height; ++y){
		for (unsigned long long x = 0; x < s.width; ++x){
			double grad = sqrt(sqr(gx.at<double>(y,x)) + sqr(gy.at<double>(y,x)));
			//PRINT(res[0].at<double>(y,x))
			//PRINT(grad)
			res[0].at<double>(y,x) = res[0].at<double>(y,x) + grad;
			res[2].at<double>(y,x) = res[2].at<double>(y,x) + grad;
		}
	}
	
	Mat ret;
	cv::merge(res,ret);
	return ret;
}

void save_hessian(unsigned long long ksize, double dev){
	Mat kernel[3];
	kernel[0] = Mat(ksize,ksize, CV_64F);
	kernel[1] = Mat(ksize,ksize, CV_64F);
	kernel[2] = Mat(ksize,ksize, CV_64F);
	Point m(ksize/2,ksize/2);
	
	Vec3d vals;
	for( unsigned long long y = 0; y < ksize; ++y){
		for (unsigned long long x = 0; x < ksize; ++x){
			(kernel[0]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)x - m.x)/dev,2) - 1.0)/(pow(dev,3)*sqrt(PI*2.0));
			(kernel[1]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev)) * ((double)x - m.x) * ((double)y - m.y)/(pow(dev,5) * sqrt(PI*2.0));
			(kernel[2]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)y - m.y)/dev,2) - 1.0)/(pow(dev,3)*sqrt(PI*2.0));
		}
	}
	
	normalize(kernel[0],kernel[0],255,0,32);
	normalize(kernel[1],kernel[1],255,0,32);
	normalize(kernel[2],kernel[2],255,0,32);
	//PRINT((kernel[0]).at<double>((int)m.y,(int)m.x))
	//PRINT((kernel[0]).at<double>(1,1))
	imwrite("a_kernel.png",kernel[0]);
	imwrite("b_kernel.png",kernel[1]);
	imwrite("c_kernel.png",kernel[2]);
}


void save_gaussian(unsigned long long ksize, double dev){
	Mat kernel;
	kernel = Mat(ksize,ksize, CV_64F);
	Point m(ksize/2,ksize/2);
	
	Vec3d vals;
	for( unsigned long long y = 0; y < ksize; ++y){
		for (unsigned long long x = 0; x < ksize; ++x){
			(kernel).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))/(pow(dev,2)*PI*2.0);
		}
	}
	
	normalize(kernel,kernel,255,0,32);
	//PRINT((kernel[0]).at<double>((int)m.y,(int)m.x))
	//PRINT((kernel[0]).at<double>(1,1))
	imwrite("gaussian_kernel.png",kernel);
}

void save_gaussian_derivatives(unsigned long long ksize, double dev){
	Mat kernel[2];
	kernel[0] = Mat(ksize,ksize, CV_64F);
	kernel[1] = Mat(ksize,ksize, CV_64F);
	Point m(ksize/2,ksize/2);
	
	Vec3d vals;
	for( unsigned long long y = 0; y < ksize; ++y){
		for (unsigned long long x = 0; x < ksize; ++x){
			(kernel[0]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*((double)x - m.x);
			(kernel[1]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*((double)y - m.y);
		}
	}
	
	normalize(kernel[0],kernel[0],255,0,32);
	normalize(kernel[1],kernel[1],255,0,32);
	//PRINT((kernel[0]).at<double>((int)m.y,(int)m.x))
	//PRINT((kernel[0]).at<double>(1,1))
	imwrite("ddx_kernel.png",kernel[0]);
	imwrite("ddy_kernel.png",kernel[1]);
}


void save_hessian_colors(unsigned long long ksize, double dev){
	vector<Mat> kernel;
	kernel.push_back(Mat(ksize,ksize, CV_64F));
	kernel.push_back(Mat(ksize,ksize, CV_64F));
	kernel.push_back(Mat(ksize,ksize, CV_64F));
	Point m(ksize/2,ksize/2);
	
	Vec3d vals;
	for( unsigned long long y = 0; y < ksize; ++y){
		for (unsigned long long x = 0; x < ksize; ++x){
			(kernel[0]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)x - m.x)/dev,2) - 1.0)/(pow(dev,3)*sqrt(PI*2.0));
			(kernel[1]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev)) * ((double)x - m.x) * ((double)y - m.y)/(pow(dev,5) * sqrt(PI*2.0));
			(kernel[2]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)y - m.y)/dev,2) - 1.0)/(pow(dev,3)*sqrt(PI*2.0));
		}
	}
	PRINT((kernel[0]).at<double>(ksize/2,ksize/2))
	PRINT((kernel[1]).at<double>(ksize/2 - dev,ksize/2 - dev))
	PRINT((kernel[2]).at<double>(ksize/2,ksize/2))
	Mat ret;
	cv::merge(kernel,ret);
	normalize(ret,ret,255,0,32);
	imwrite("color_kernel.png",ret);
}


Mat convolve_modified_hessian(Mat img, unsigned long long ksize, double dev){
	Mat kernel[3];
	kernel[0] = Mat(ksize,ksize, CV_64F);
	kernel[1] = Mat(ksize,ksize, CV_64F);
	kernel[2] = Mat(ksize,ksize, CV_64F);
	Point m(ksize/2,ksize/2);
	
	Vec3d vals;
	for( unsigned long long y = 0; y < ksize; ++y){
		for (unsigned long long x = 0; x < ksize; ++x){
			(kernel[0]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)x - m.x)/dev,2) - 1.0)/(pow(dev,2)*sqrt(PI*2.0)) - exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)y - m.y)/dev,2) - 1.0)/(3.0 * pow(dev,2)*sqrt(PI*2.0));
			(kernel[1]).at<double>(y,x) = 1.33333333333333333333 * exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev)) * ((double)x - m.x) * ((double)y - m.y)/(pow(dev,4) * sqrt(PI*2.0));
			(kernel[2]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)y - m.y)/dev,2) - 1.0)/(pow(dev,2)*sqrt(PI*2.0)) - exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)x - m.x)/dev,2) - 1.0)/(3.0 * pow(dev,2)*sqrt(PI*2.0));
		}
	}
	vector<Mat> res;
	Mat R1;
	Mat R2;
	Mat R3;
	res.push_back(R1);
	res.push_back(R2);
	res.push_back(R3);
	
	filter2D(img, res[0], -1 ,kernel[0], m, 0, BORDER_DEFAULT);
	filter2D(img, res[1], -1 ,kernel[1], m, 0, BORDER_DEFAULT);
	filter2D(img, res[2], -1 ,kernel[2], m, 0, BORDER_DEFAULT);
	Mat ret;
	cv::merge(res,ret);
	return ret;
}

Mat tubeness_hessian(Mat hes){
	Size s = hes.size();
	Mat ret(s.height, s.width, CV_64F);
	Vec3d vals;
	for (unsigned long long y = 0; y < s.height; ++y){
		for (unsigned long long x = 0; x < s.width; ++x){
			vals = hes.at<Vec3d>(y,x);
			ret.at<double>(y,x) = -(vals[0] + vals[2] - sqrt(pow(vals[0] - vals[2],2) + 4.0 * pow(vals[1],2)))/2.0;
		}
	}
	return ret;
}

Mat convertHSVtoBGR(Mat img){
	Size s = img.size();
	Mat ret(s.height,s.width, CV_64FC3);
	uint8_t hi;
	double f;
	double p;
	double q;
	double t;
	Vec3d BGR;
	Vec3d vals;
	for (unsigned long long y = 0; y < s.height; ++y){
		for (unsigned long long x = 0; x < s.width; ++x){
			vals = img.at<Vec3d>(y,x);
			hi = vals[0] / 60;
			f = vals[0] / 60.0 - hi;
			p = vals[2] * (1.0 - vals[1]);
			q = vals[2] * (1.0 - vals[1] * f);
			t = vals[2] * (1.0 - vals[1] * (1.0 - f));
			switch(hi) {
				case 0: BGR[0] = p; BGR[1] = t; BGR[2] = vals[2]; break;
				case 1: BGR[0] = p; BGR[1] = vals[2]; BGR[2] = q; break;
				case 2: BGR[0] = t; BGR[1] = vals[2]; BGR[2] = p; break;
				case 3: BGR[0] = vals[2]; BGR[1] = q; BGR[2] = p; break;
				case 4: BGR[0] = vals[2]; BGR[1] = p; BGR[2] = t; break;
				case 5: BGR[0] = q; BGR[1] = p; BGR[2] = vals[2]; break;
				case 6: BGR[0] = p; BGR[1] = t; BGR[2] = vals[2]; break;
			}
			ret.at<Vec3d>(y,x) = BGR;
		}
	}
	return ret;
}

Mat visualize_hessian(Mat hes){
	Size s = hes.size();
	Mat ret(s.height,s.width, CV_64FC3);
	Mat H(s.height,s.width, CV_64F);
	Mat S(s.height,s.width, CV_64F);
	Mat V(s.height,s.width, CV_64F);
	Vec3d vals;
	double l1;
	double l2;
	double maxV = -numeric_limits<double>::infinity();
	double minV = numeric_limits<double>::infinity();
	
//	double maxH = -numeric_limits<double>::infinity();
//	double minH = numeric_limits<double>::infinity();
	
	for (unsigned long long y = 0; y < s.height; ++y){
		for (unsigned long long x = 0; x < s.width; ++x){
			vals = hes.at<Vec3d>(y,x);
			//not optimal performance, but i dont give a shit atm
			l1 = (vals[0] + vals[2] - sqrt(pow(vals[0] - vals[2],2) + 4.0 * pow(vals[1],2)))/2.0;
			l2 = (vals[0] + vals[2] + sqrt(pow(vals[0] - vals[2],2) + 4.0 * pow(vals[1],2)))/2.0;
			H.at<double>(y,x) = (atan2(vals[0] - vals[2] - sqrt(pow(vals[0] - vals[2],2) + 4.0 * pow(vals[1],2)),2.0 * vals[1])) * 360.0/PI + 360;
			while (H.at<double>(y,x) >= 360.0){
				H.at<double>(y,x) -= 360.0;
			}
			if (H.at<double>(y,x) < 0.0){
				PRINT(H.at<double>(y,x))
			}
			if (H.at<double>(y,x) > 360.0){
				PRINT(H.at<double>(y,x))
			}
			S.at<double>(y,x) = (abs(l2) - abs(l1))/(abs(l1) + abs(l2));
			V.at<double>(y,x) = -l1;
			
			if (maxV < -l1){
				maxV = -l1;
			}
			if (minV > -l1){
				minV = -l1;
			}
			/*
			if (maxH < H.at<double>(y,x)){
				maxH = H.at<double>(y,x);
			}
			if (minH > H.at<double>(y,x)){
				minH = H.at<double>(y,x);
			}*/

		}
	}
	
//	PRINT(minH)
//	PRINT(maxH)
	
	V = V - minV;
	V = V / (maxV - minV);
	vector<Mat> HSV;
	HSV.push_back(H);
	HSV.push_back(S);
	HSV.push_back(V);
	cv::merge(HSV,ret);
	//PRINT(ret.channels())
	ret = convertHSVtoBGR(ret);
	//PRINT(ret.channels())
	normalize(ret,ret,255,0,32); // the 32 is a flag, that produced errors if the keyword is used directly
	//PRINT(ret.channels())
	return ret;
}

#define bins 180
void hessian_weighted_angle_distribution(Mat hes, unsigned long long border){
	Size s = hes.size();
	double w;
	double a;
	double hist[bins];
	memset(hist, 0.0, bins * sizeof(hist[0]));
	Vec3d vals;
	for (unsigned long long y = border; y < s.height - border; ++y){
		for (unsigned long long x = border; x < s.width - border; ++x){
			vals = hes.at<Vec3d>(y,x);
			w = min((vals[0] + vals[2] - sqrt(pow(vals[0] - vals[2],2) + 4.0 * pow(vals[1],2)))/2.0,0.0);
			a = (atan2(vals[0] - vals[2] - sqrt(pow(vals[0] - vals[2],2) + 4.0 * pow(vals[1],2)),2.0 * vals[1])) * 180.0/PI + 180;
			while (a >= 180.0){
				a -= 180.0;
			}
			hist[(int)a] -= w;
		}
	}
	for (int i = 0; i < 180; ++i){
		cout << i << " " << hist[i] << endl;
	}
}

void hessian_weighted_relative_angle_distribution(Mat hes, double dev, unsigned long long border){
	Size s = hes.size();
	//int bins = 180;
	double W;
	double hist[bins] = {0.0};
	double hist_rel[bins/2] = {0.0};
	Vec3d vals;
	
	Mat a(s.height,s.width, CV_64F);
	Mat w(s.height,s.width, CV_64F);
	
	for (unsigned long long y = 0; y < s.height; ++y){
		for (unsigned long long x = 0; x < s.width; ++x){
			vals = hes.at<Vec3d>(y,x);
			w.at<double>(y,x) = min((vals[0] + vals[2] - sqrt(pow(vals[0] - vals[2],2) + 4.0 * pow(vals[1],2)))/2.0,0.0);
			a.at<double>(y,x) = (atan2(vals[0] - vals[2] - sqrt(pow(vals[0] - vals[2],2) + 4.0 * pow(vals[1],2)),2.0 * vals[1])) * 180.0/PI + 180;
			while (a.at<double>(y,x) >= 180.0){
				a.at<double>(y,x) -= 180.0;
			}
		}
	}
	for (unsigned long long y0 = border; y0 < s.height - border; ++y0){
//		PRINT(y0)
		for (unsigned long long x0 = border; x0 < s.width - border; ++x0){
			memset(hist, 0, bins * sizeof(hist[0]));
			for (unsigned long long y = max(0,(int)(y0 - 2.0*dev)); y < min(s.height,(int)(y0 + 2.0*dev)); ++y){
				for (unsigned long long x = max(0,(int)(x0 - 2.0*dev)); x < min(s.width,(int)(x0 + 2.0*dev)); ++x){
					W = w.at<double>(y,x) * exp(-(pow(x - x0,2) + pow(y - y0,2))/pow(dev * SQRT2, 2));
					hist[(int)a.at<double>(y,x)] -= W;
				}
			}
			for (int i = 0; i < bins; ++i){
				for(int j = 0; j < bins/2; ++j){
					hist_rel[j] += pow(hist[i] * hist[(i + j)%bins],2);
				}
			}
		}
	}
	for(int j = 0; j < bins/2; ++j){
		cout << hist_rel[j] << endl;
	}
}


// clamps a Mat between lower and upper (if a value is outside the bounds of lower and upper, it is set to lower or upper respectively)
void clamp(Mat &mat, double lower, double upper) {
    min(max(mat, lower), upper, mat);
}

// retrns a "function" that maps the intensety of a pixel inside a certain radius range to their angle to the x axis. the function consists of 2 vectors in a array, where the first vector carrys the angle and the second carrys the pixel value.
vector<double>* circlefun(Mat* img, double xpos, double ypos, double inner, double outer){
	vector<double>* fun = new vector<double>[2];
	Size s = img->size();
	unsigned long long xmin = max((unsigned long long)0,(unsigned long long)floor(xpos - outer));
	unsigned long long xmax = min((unsigned long long)ceil(xpos + outer), (unsigned long long)s.width);
	unsigned long long ymin = max((unsigned long long)0,(unsigned long long)floor(ypos - outer));
	unsigned long long ymax = min((unsigned long long)ceil(ypos + outer), (unsigned long long)s.height);
	double rad = 0;
	for (unsigned long long x = xmin; x < xmax ; ++x){
		for (unsigned long long y = ymin; y < ymax; ++y){
			rad = sqrt(pow((double)x-xpos,2) + pow((double)y-ypos,2));
			if (inner <= rad && rad <= outer && x != xpos && y != ypos){
				(fun[0]).push_back(atan2((double)y-ypos,(double)x-xpos));
				(fun[1]).push_back(img->at<double>(y,x));
//				cout << (fun[0]).back() << " " << (fun[1]).back() << " " << x << " " << y << " " << rad << endl; //to test the function
			}
		}
	}
	
	return fun;
}

vector<double>* circlefun_hessian(Mat* img, double xpos, double ypos, double inner, double outer){
	vector<double>* fun = new vector<double>[2];
	Size s = img->size();
	unsigned long long xmin = max((unsigned long long)0,(unsigned long long)floor(xpos - outer));
	unsigned long long xmax = min((unsigned long long)ceil(xpos + outer), (unsigned long long)s.width - (unsigned long long)1);
	unsigned long long ymin = max((unsigned long long)0,(unsigned long long)floor(ypos - outer));
	unsigned long long ymax = min((unsigned long long)ceil(ypos + outer), (unsigned long long)s.height - (unsigned long long)1);
	double rad = 0;
	double radsqr = 0;
	double dx;
	double dy;
	Vec3d vals;
	for (unsigned long long x = xmin; x <= xmax ; ++x){
		for (unsigned long long y = ymin; y <= ymax; ++y){
			dx = (double)x - xpos;
			dy = (double)y - ypos;
			radsqr = pow(dx,2) + pow(dy,2);
			rad = sqrt(radsqr);
			if (inner <= rad && rad <= outer && x != xpos && y != ypos){
				vals = img->at<Vec3d>(y,x);
				(fun[0]).push_back(atan2(dy,dx));
				(fun[1]).push_back(-(pow(dy,2)*vals[0] - 2.0 * vals[1] * dx * dy + vals[2] * pow(dx,2))/radsqr);
				//cout << (fun[0]).back() << " " << (fun[1]).back() << endl;// << " " << x << " " << y << " " << rad << endl; //to test the function
			}
		}
	}
	
	return fun;
}


// specifically designed to take the output of circlefun in the first argument. the function fun has to be a array containing 2 vectors. the first vector has to contain the angle between -pi and pi, and tha 2nd vector the assosiated value. this function produces equidistant gaussian weighted averages in "steps" (2nd argument) positions between -pi and pi, where the first value is -pi and the last slightly smaller than pi, because pi and - pi are equivalent, and the value would be dublicated. the standart deviation of the normal distribution used to weigh the values of fun, is given by "dev" the 3rd argument. the last argument is used to stop the function to free the memory occupied by "fun", since it is usually no longer required.
double** gaussavgcircle(vector<double>* fun,unsigned long long steps,double dev, bool free = 1){
	double** avg = new double*[2];
	avg[0] = new double[steps];
	avg[1] = new double[steps];
	double acc_val;
	double acc_dense;
	double val;
	for ( unsigned long long i = 0; i < steps; ++i){
		avg[0][i] = ((double)i * 2.0 * PI / (double)steps) - PI;
	}
	
	for ( unsigned long long i = 0; i < steps; ++i){
		acc_val = 0;
		acc_dense = 0;
		val = 0;
		
		for ( unsigned long long j = 0; j < (fun[0]).size(); ++j){
			val = exp(-pow(fun[0][j] - avg[0][i],2)/pow(dev * SQRT2, 2)); 
			val += exp(-pow(fun[0][j] - avg[0][i] - (2 * PI),2)/pow(dev * SQRT2, 2));
			val += exp(-pow(fun[0][j] - avg[0][i] + (2 * PI),2)/pow(dev * SQRT2, 2));
			acc_dense += val;
			acc_val += val * fun[1][j];
		}
		if (acc_val == 0){
			avg[1][i] = 0;
		}
		else {
			avg[1][i] = acc_val/acc_dense;
		}
//		cout << avg[0][i] << " " << avg[1][i] << endl; //to test the function
		
	}
	
	if (free) {
		delete[] fun;
	}
	
	return avg;
}

void calc_avgs(double** avg, vector<double>* fun, double dev, unsigned long long i_start, unsigned long long i_end){
	double acc_val;
	double acc_dense;
	double val;
	
	for (unsigned long long i = i_start; i < i_end; ++i){
		acc_val = 0;
		acc_dense = 0;
		val = 0;
		
		for ( unsigned long long j = 0; j < (fun[0]).size(); ++j){
			val = exp(-pow(fun[0][j] - avg[0][i],2)/pow(dev * SQRT2, 2)); 
			val += exp(-pow(fun[0][j] - avg[0][i] - (2 * PI),2)/pow(dev * SQRT2, 2));
			val += exp(-pow(fun[0][j] - avg[0][i] + (2 * PI),2)/pow(dev * SQRT2, 2));
			acc_dense += val;
			acc_val += val * fun[1][j];
		}
		if (acc_val == 0){
			avg[1][i] = 0;
		}
		else {
			avg[1][i] = acc_val/acc_dense;
		}
	}
}

double** gaussavgcircle_MT(vector<double>* fun,unsigned long long steps,double dev, bool free = 1){
	double** avg = new double*[2];
	avg[0] = new double[steps];
	avg[1] = new double[steps];
	double acc_val;
	double acc_dense;
	double val;
	for ( unsigned long long i = 0; i < steps; ++i){
		avg[0][i] = ((double)i * 2.0 * PI / (double)steps) - PI;
	}
	
//	unsigned long long thread_borders[THREADS + 1];
	
	unsigned long long* thread_borders = new unsigned long long[THREADS + 1];
	
	double thread_steps = (double)steps/(double)THREADS;
	for (unsigned long long i = 0; i < THREADS; ++i){
		thread_borders[i] =(unsigned long long)((double)i * (double)thread_steps);
	}
	thread_borders[THREADS] = steps;
	
//	thread t[THREADS];
	
	thread* t = new thread[THREADS];
	
	for (unsigned long long i = 0; i < THREADS; ++i){
		t[i] = thread(calc_avgs,avg, fun, dev, thread_borders[i], thread_borders[i+1]);
	}
	
	for (unsigned long long i = 0; i < THREADS; ++i){
		t[i].join();
	}
	
	/*
	for ( unsigned long long i = 0; i < steps; ++i){
		acc_val = 0;
		acc_dense = 0;
		val = 0;
		
		for ( unsigned long long j = 0; j < (fun[0]).size(); ++j){
			val = exp(-pow(fun[0][j] - avg[0][i],2)/pow(dev * SQRT2, 2)); 
			val += exp(-pow(fun[0][j] - avg[0][i] - (2 * PI),2)/pow(dev * SQRT2, 2));
			val += exp(-pow(fun[0][j] - avg[0][i] + (2 * PI),2)/pow(dev * SQRT2, 2));
			acc_dense += val;
			acc_val += val * fun[1][j];
		}
		if (acc_val == 0){
			avg[1][i] = 0;
		}
		else {
			avg[1][i] = acc_val/acc_dense;
		}
//		cout << avg[0][i] << " " << avg[1][i] << endl; //to test the function
		
	}
	*/
	
	delete[] t;
	delete[] thread_borders;
	
	if (free) {
		delete[] fun;
	}
	
	return avg;
}


//this function is specificaly designed to take the output of gaussavgcircle. it looks for local maxima in the input vector and outputs their locations in a vector. it also needs the length of the array, since in c++ it can not be determined during runtime. usually the length would be the number of steps in the gaussavgcircle function.
vector<unsigned long long> findpks(double* vals,unsigned long long length, bool free = 1){
	vector<unsigned long long> pks;
	
	if (vals[1] < vals[0] && vals[0] > vals[length - 1]){
		pks.push_back(0);
	}
	for (unsigned long long v = 1; v < length - 2; ++v){
		if (vals[v - 1] < vals[v] && vals[v] > vals[v+1]){
			pks.push_back(v);
		}
	}
	
	if (vals[length - 2] < vals[length - 1] && vals[length - 1] > vals[0]){
		pks.push_back(length - 1);
	}
	
	return pks;
}

vector<unsigned long long> findpks_thresh(double* vals,unsigned long long length,double thresh = 0.0, bool free = 1){
	vector<unsigned long long> pks;

	/*
	vector<double> vec_vals;
	for(int i = 0; i < length; ++i){
		vec_vals.push_back(vals[i]);
	}
	double_vector_to_file(to_string(rand()%1024) + ".ang",vec_vals);
	*/
	
	if (vals[1] < vals[0] && vals[0] > vals[length - 1] && vals[0] > thresh){
		pks.push_back(0);
	}
	for (unsigned long long v = 1; v < length - 2; ++v){
		if (vals[v - 1] < vals[v] && vals[v] > vals[v+1] && vals[v] > thresh){
			pks.push_back(v);
		}
	}
	
	if (vals[length - 2] < vals[length - 1] && vals[length - 1] > vals[0] && vals[length - 1] > thresh){
		pks.push_back(length - 1);
	}
	
	return pks;
}


//function to get the intensity of a image along a loing with a certain thicness
vector<double>* linefun(Mat* img, double xstart, double ystart, double xend, double yend, double thic){
	vector<double>* fun = new vector<double>[2];
	Size s = img->size();
	
	double dx = xend - xstart;
	double dy = yend - ystart;
	double len = sqrt(pow(dx,2) + pow(dy,2));
	
	double e1x = xstart + thic*0.5*dy/len;
	double e1y = ystart - thic*0.5*dx/len;
	double e2x = xstart - thic*0.5*dy/len;
	double e2y = ystart + thic*0.5*dx/len;
	double e3x = xend - thic*0.5*dy/len;
	double e3y = yend + thic*0.5*dx/len;
	double e4x = xend + thic*0.5*dy/len;
	double e4y = yend - thic*0.5*dx/len;
	
	
	unsigned long long xmax = ceil(min(max(max(e1x,e2x),max(e3x,e4x)),(double)s.width));
	unsigned long long xmin = floor(max(min(min(e1x,e2x),min(e3x,e4x)),0.0));
	unsigned long long ymax = ceil(min(max(max(e1y,e2y),max(e3y,e4y)),(double)s.height));
	unsigned long long ymin = floor(max(min(min(e1y,e2y),min(e3y,e4y)),0.0));
	
	for ( unsigned long long x = xmin; x <= xmax; ++x){
		for (unsigned long long y = ymin; y <= ymax; ++y){
			if ((y-e2y)*(e3x-e2x) <= (e3y-e2y)*(x-e2x) && (y-e1y)*(e4x-e1x) >= (e4y-e1y)*(x-e1x) && (y-e1y)*(e2x-e1x) <= (e2y-e1y)*(x-e1x) && (y-e4y)*(e3x-e4x) >= (e3y-e4y)*(x-e4x)){
				fun[0].push_back((((x - xstart) * dx) + ((y - ystart) * dy))/len);
				fun[1].push_back(img->at<double>(y,x));
//				cout << fun[0].back() << " " << fun[1].back() << endl;
			}
		}
	}
	return fun;
}



double linefun_score_hessian(Mat* img, double xstart, double ystart, double xend, double yend, double thic){
	double score = 0;
	Size s = img->size();
	
	double dx = xend - xstart;
	double dy = yend - ystart;
	double len = sqrt(pow(dx,2) + pow(dy,2));
	
	double e1x = xstart + thic*0.5*dy/len;
	double e1y = ystart - thic*0.5*dx/len;
	double e2x = xstart - thic*0.5*dy/len;
	double e2y = ystart + thic*0.5*dx/len;
	double e3x = xend - thic*0.5*dy/len;
	double e3y = yend + thic*0.5*dx/len;
	double e4x = xend + thic*0.5*dy/len;
	double e4y = yend - thic*0.5*dx/len;
	
	
	unsigned long long xmax = ceil(min(max(max(e1x,e2x),max(e3x,e4x)),(double)s.width - 1));
	unsigned long long xmin = floor(max(min(min(e1x,e2x),min(e3x,e4x)),0.0));
	unsigned long long ymax = ceil(min(max(max(e1y,e2y),max(e3y,e4y)),(double)s.height - 1));
	unsigned long long ymin = floor(max(min(min(e1y,e2y),min(e3y,e4y)),0.0));
	
	Vec3d vals;

	for ( unsigned long long x = xmin; x <= xmax; ++x){
		for (unsigned long long y = ymin; y <= ymax; ++y){
			if ((y-e2y)*(e3x-e2x) <= (e3y-e2y)*(x-e2x) && (y-e1y)*(e4x-e1x) >= (e4y-e1y)*(x-e1x) && (y-e1y)*(e2x-e1x) <= (e2y-e1y)*(x-e1x) && (y-e4y)*(e3x-e4x) >= (e3y-e4y)*(x-e4x)){
				vals = img->at<Vec3d>(y,x);
				score += -(pow(dy,2)*vals[0] - 2.0 * vals[1] * dx * dy + vals[2] * pow(dx,2));
			}
		}
	}
	return score/pow(len,3);
}

double score_line(vector<node*> line, double thick){
	double score = 0;
	if (line.size() < 2){return score;}
	Mat* img = line[0]->img;
	for (unsigned long long i = 0; i < line.size() - 1; ++i){
		score += linefun_score_hessian(img,line[i]->x, line[i]->y, line[i+1]->x, line[i+1]->y, thick);
	}
	score = score/((double)(line.size() - 1));

	return score;
}

bool gaussavgoverthresh(vector<double>* fun,double length,unsigned long long steps,double dev, double thresh, bool free = 1){
	double avgx[steps];
	bool overthresh = true;
	double acc_val;
	double acc_dense;
	double val;
	for ( unsigned long long i = 0; i < steps; ++i){
		avgx[i] = ((double)i * length / (double)steps);
	}
	
	for ( unsigned long long i = 0; i < steps && overthresh; ++i){
		acc_val = 0;
		acc_dense = 0;
		val = 0;
		
		for ( unsigned long long j = 0; j < (fun[0]).size(); ++j){
			val = exp(-pow(fun[0][j] - avgx[i],2)/pow(dev * SQRT2, 2)); //the 1.414... is the sqrt(2) and saves computing time.
			acc_dense += val;
			acc_val += val * fun[1][j];
		}
		if (acc_val == 0){
			overthresh = false;
//			PRINT(acc_val)
		}
		else if (acc_val/acc_dense < thresh) {
			overthresh = false;
//			PRINT(overthresh)
		}
//		cout << avg[0][i] << " " << avg[1][i] << endl; //to test the function
		
	}
	
	if (free) {
		delete[] fun;
	}
	
	return overthresh;
}

double rel_angle(double dx1, double dy1, double dx2, double dy2){
	double angle = atan2(dx2,dy2) - atan2(dx1,dy1);
	while (angle < 0) {
			angle = angle +  (2.0 * PI);
	}
	while (angle >= (2.0 * PI)){
		angle = angle - (2.0 * PI);
	}
	return angle;
}

double rel_angle(double dx1, double dy1, double a2){
	double angle = a2 - atan2(dx1,dy1);
	while (angle < 0) {
			angle = angle +  (2.0 * PI);
	}
	while (angle >= (2.0 * PI)){
		angle = angle - (2.0 * PI);
	}
	return angle;
}

double rel_half_angle(double dx1, double dy1, double dx2, double dy2){
	double angle = rel_angle(dx1, dy1, dx2, dy2);
	if (angle > PI){
		angle = abs(angle - (2 * PI));
	}
	return angle;
}

double rel_half_angle(double dx1, double dy1, double a2){
	double angle = rel_angle(dx1, dy1, a2);
	if (angle > PI){
		angle = abs(angle - (2 * PI));
	}
	return angle;
}


double rel_quater_angle(double dx1, double dy1, double dx2, double dy2){
	double angle = rel_half_angle(dx1, dy1, dx2, dy2);
	if (angle > PI * 0.5){
		angle = abs(angle - PI);
	}
	return angle;
}

bool find_intersect(double* crosspar, double x11,double y11,double x12,double y12,double x21,double y21,double x22,double y22){
	bool parallel = false;
	double det = (x11-x12)*(y21-y22)-(y11-y12)*(x21-x22);
	if (det == 0.0){ // lines are parallel
		crosspar[0] = numeric_limits<double>::infinity();
		crosspar[1] = numeric_limits<double>::infinity();
		crosspar[2] = numeric_limits<double>::infinity();
		crosspar[3] = numeric_limits<double>::infinity();
		parallel = true;
	}
	else {
		crosspar[2] = ((x11-x21)*(y21-y22) - (y11-y21)*(x21-x22))/det;
		crosspar[3] = ((y11-y12)*(x11-x21) - (x11-x12)*(y11-y21))/det;
		crosspar[0] = x11 + crosspar[2]*(x12-x11);
		crosspar[1] = y21 + crosspar[3]*(y22-y21);
		//crosspar[1] = y11 + crosspar[2]*(y12-y11);
	}
	return parallel;
}

#endif
