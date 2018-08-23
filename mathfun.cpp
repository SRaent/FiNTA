#ifndef MATHFUN_CPP
#define MATHFUN_CPP MATHFUN_CPP

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


#define PI (double)3.1415926535897932384626433832795



// takes a 2 dimensional array of uint8 representing a image(as generated by importsemtif), the with and hight of the image and fouriertransforms it. the output has to be dereferenced in the form [(x*hight) + y] since it is a single dimensional array.
fftw_complex* fft_2d(uint8** img,unsigned long long w, unsigned long long h){
	
	fftw_complex *in, *out;
	fftw_plan p;
	
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * w * h);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * w * h);
	
	p = fftw_plan_dft_2d(w,h,in,out,FFTW_FORWARD, FFTW_ESTIMATE);
	
	for(unsigned long long i = 0; i < w; ++i){
		for (unsigned long long j = 0; j < h; ++j){
			in[(i * h) + j][0] =  (double)img[i][j]; // the in[(i * h) + j][0] referes to the real part of the fftw_complex data type
		}
	}
	
	fftw_execute(p);
	
	fftw_destroy_plan(p);
	fftw_free(in);
	return out;
}



fftw_complex* fft_2d(const Mat &img){
	
	fftw_complex *in, *out;
	fftw_plan p;
	
	Size s = img.size();
	
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * s.width * s.height);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * s.width * s.height);
	
	p = fftw_plan_dft_2d(s.width,s.height,in,out,FFTW_FORWARD, FFTW_ESTIMATE);
	
	
	for(unsigned long long i = 0; i < s.height; ++i){
		for (unsigned long long j = 0; j < s.width; ++j){
			in[(j * s.height) + i][0] = img.at<double>(i,j); // the in[(i * h) + j][0] referes to the real part of the fftw_complex data type
		}
	}
	
	fftw_execute(p);
	
	fftw_destroy_plan(p);
	fftw_free(in);
	return out;
}




// takes a array of fftw_complex to be dereferenced by[(x*hight) + y] (as generated by fft_2d) with hight h and with w and inverse fourier transforms it. it also frees the memory of the input array when free == 1.
fftw_complex* ifft_2d(fftw_complex* in, unsigned long long w, unsigned long long h, bool free = 1){
	fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * w * h);
	fftw_complex *buffer = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * w * h); // zhis haz to be done!!! because creating ze plan can overwrite both arrays, which would be horrible if the actual input data was used here and overwritten. if the output is overwirtten i dont care because it will be overwitten again with the correct output anyway
	fftw_plan p = fftw_plan_dft_2d(w,h,buffer,out,FFTW_BACKWARD,FFTW_ESTIMATE);
	fftw_free(buffer);
	
	fftw_execute_dft(p,in,out); //execute the plan but on the actual input data not the array used to create it.
	
	
	
	fftw_destroy_plan(p);
	if(free) {
		fftw_free(in);
	}
	return out;
	
}




// like ifft_2d but it outputs the data in a nice 2D array od real values.
double** ifft_2d_real(fftw_complex* in, unsigned long long w, unsigned long long h, bool free = 1){
	fftw_complex* comp = ifft_2d(in,w,h);
	
	double** img = new double*[w];
	
	for(unsigned long long x = 0; x < w; ++x){
		img[x] = new double[h];
		for(unsigned long long y = 0; y < h; ++y){
			img[x][y] = comp[(x * h) + y][0] /(w * h); // the ffwt library does not normalize the data so the data has to be devided by w*h
		}
	}
	
	if (free){
		fftw_free(comp);
	}
	return img;
}
//inverse fourier transform returning a Mat object. argiments: array to be reverse fourier transformed, image width, image height, dealloccate array to be reverse fourier transformed when done?
Mat cv_ifft_2d_real(fftw_complex* in, unsigned long long w, unsigned long long h, bool free = 1){
	fftw_complex* comp = ifft_2d(in,w,h);
	
	Mat img(h,w,CV_64F,Scalar::all(0));
	
	for(unsigned long long x = 0; x < w; ++x){
		for(unsigned long long y = 0; y < h; ++y){
			img.at<double>(y,x) = comp[(x * h) + y][0] /((double)w * (double)h);
		}
	}
	
	if (free){
		fftw_free(comp);
	}
	return img;
}


Mat convolve_hessian(Mat img, unsigned long long ksize, double dev){
	Mat kernel[3];
	kernel[0] = Mat(ksize,ksize, CV_64F);
	kernel[1] = Mat(ksize,ksize, CV_64F);
	kernel[2] = Mat(ksize,ksize, CV_64F);
	Point m(ksize/2,ksize/2);
	
	Vec3d vals;
	for( unsigned long long y = 0; y < ksize; ++y){
		for (unsigned long long x = 0; x < ksize; ++x){
			(kernel[0]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)x - m.x)/dev,2) - 1.0)/(2.0*pow(dev,4)*PI);
			(kernel[1]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev)) * ((double)x - m.x) * ((double)y - m.y)/(2.0 * pow(dev,6) * PI);
			(kernel[2]).at<double>(y,x) = exp(-(pow(((double)x - m.x),2) + pow(((double)y - m.y),2))/(2.0*dev*dev))*(pow(((double)y - m.y)/dev,2) - 1.0)/(2.0*pow(dev,4)*PI);
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
	uint8 hi;
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
	ret = convertHSVtoBGR(ret);
	return ret;
}

void hessian_weighted_angle_distribution(Mat hes, unsigned long long border){
	Size s = hes.size();
	int bins = 180;
	double w;
	double a;
	double hist[bins] = {0.0};
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
	int bins = 180;
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
// sets all pixels of a image img to 0 that are inside a ellipse with "radius" rad, while taking the "topology" of a discrete fourier transfrom into aount (the center where the frequency is 0 is devidet between the 4 corners). also the ellips is streched according to the aspect ratio of the image given by w and h (width and hight).
void cutinneroval_ft(fftw_complex* img,double rad,unsigned long w, unsigned long h){
	rad = pow((double)h * rad / 2.0,2);
	double rel = (double)h / (double)w;
	double xcent = (double)w/2.0;
	double ycent = (double)h/2.0;
	for(unsigned long x = 0; x < w; ++x){
		for(unsigned long y = 0; y < h; ++y){
			if ((x <= xcent && y <= ycent && (pow((x * rel),2) + pow(y,2) < rad)) || (x >= xcent && y <= ycent && pow(((w - x) * rel),2) + pow(y,2) < rad) || (x <= xcent && y >= ycent && pow((x * rel),2) + pow((h - y),2) < rad) || (x >= xcent && y >= ycent && pow(((w - x) * rel),2) + pow((h - y),2) < rad)){
				img[(x * h) + y][0] = 0;
				img[(x * h) + y][1] = 0;
			}
		}
	}
}
//almost same as cutinneroval_ft only the outside of the oval is set to 0
void cutouteroval_ft(fftw_complex* img,double rad,unsigned long w, unsigned long h){
	rad = pow((double)h * rad / 2.0,2);
	double rel = (double)h / (double)w;
	double xcent = (double)w/2.0;
	double ycent = (double)h/2.0;
	for(unsigned long x = 0; x < w; ++x){
		for(unsigned long y = 0; y < h; ++y){
			if ((x <= xcent && y <= ycent && (pow((x * rel),2) + pow(y,2) > rad)) || (x >= xcent && y <= ycent && pow(((w - x) * rel),2) + pow(y,2) > rad) || (x <= xcent && y >= ycent && pow((x * rel),2) + pow((h - y),2) > rad) || (x >= xcent && y >= ycent && pow(((w - x) * rel),2) + pow((h - y),2) > rad)){
				img[(x * h) + y][0] = 0;
				img[(x * h) + y][1] = 0;
			}
		}
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
	for (unsigned long long x = xmin; x <= xmax ; ++x){
		for (unsigned long long y = ymin; y <= ymax; ++y){
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
	unsigned long long xmax = min((unsigned long long)ceil(xpos + outer), (unsigned long long)s.width);
	unsigned long long ymin = max((unsigned long long)0,(unsigned long long)floor(ypos - outer));
	unsigned long long ymax = min((unsigned long long)ceil(ypos + outer), (unsigned long long)s.height);
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
//				cout << (fun[0]).back() << " " << (fun[1]).back() << " " << x << " " << y << " " << rad << endl; //to test the function
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
			val = exp(-pow(fun[0][j] - avg[0][i],2)/pow(dev * SQRT2, 2)); //the 1.414... is the sqrt(2) and saves computing time.
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

#endif