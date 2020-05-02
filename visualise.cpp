#ifndef VISUALISE_CPP
#define VISUALISE_CPP VISUALISE_CPP
#include "userinterface.cpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/types.hpp"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "mathfun.cpp"
#include "node.h"
#include "node.cpp"
#include "analyse.cpp"

#define PI (double)3.1415926535897932384626433832795

Mat draw_list(Mat input_image, vector<node*>list,Scalar color = Scalar(0,255,0),double thickness = 1){
	Mat image;
	input_image.copyTo(image);
	for (unsigned long long i = 0; i < list.size(); ++i){
		for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
			line(image,Point(list[i]->x,list[i]->y),Point(list[i]->connections[j]->x,list[i]->connections[j]->y),color, thickness, LINE_AA);
			
		}
	}
	return image;
}

Mat draw_list_offset(Mat input_image, vector<node*>list,Scalar color = Scalar(0,255,0),double thickness = 1, double off_x = 0, double off_y = 0){
	Mat image;
	input_image.copyTo(image);
	for (unsigned long long i = 0; i < list.size(); ++i){
		for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
			line(image,Point(list[i]->x+off_x,list[i]->y+off_y),Point(list[i]->connections[j]->x+off_x,list[i]->connections[j]->y+off_y),color, thickness, LINE_AA);
			
		}
	}
	return image;
}

Mat draw_loop(vector<node*> loop, Mat image, Scalar color = Scalar(0,0,255)){
	if (image.channels() == 1){
		cvtColor(image,image,COLOR_GRAY2BGR);
	}
	for (unsigned long long i = 0; i < loop.size() - 1; ++i){
		line(image,Point(loop[i]->x,loop[i]->y),Point(loop[i + 1]->x,loop[i + 1]->y),color);
	}
	return image;
}


void draw_loops(char* folder, vector<vector<node*>> loops, Mat image,bool print_loop_params = false){
	Mat loops_img;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		image.copyTo(loops_img);
		if (print_loop_params){
			char area[100];
			sprintf(area,"area = %f; length = %f",loop_area(loops[l]),loop_length(loops[l]));
			putText(loops_img,area, Point(20,20), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0,255,127));
		}
		
		for (unsigned long long i = 0; i < loops[l].size() - 1; ++i){
			line(loops_img,Point(loops[l][i]->x,loops[l][i]->y),Point(loops[l][i + 1]->x,loops[l][i + 1]->y),Scalar(255, 255 * i / loops[l].size(), 255));
		}
		char filename[500];
		sprintf(filename,"%sloop%d.tif",folder,(int)l);
		//cout << filename << endl;
		imwrite(filename,loops_img);
	}
}

Mat draw_closures(Mat input_image, vector<node**> closures,Scalar color = Scalar(0,0,255)){
	Mat image;
	input_image.copyTo(image);
	for (unsigned long long i = 0; i < closures.size(); ++i){
		line(image,Point(closures[i][0]->x,closures[i][0]->y),Point(closures[i][1]->x,closures[i][1]->y),color);
	}
	return image;
}

Mat draw_scale_list(Mat input_image, vector<node*> list, double scale, Scalar color = Scalar(0,255,0),double thickness = 1){
	Mat image;
	input_image.copyTo(image);
	cv::resize(image, image, Size(0,0), scale, scale);
	for (unsigned long long i = 0; i < list.size(); ++i){
		for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
			line(image,Point(scale*list[i]->x,scale*list[i]->y),Point(scale*list[i]->connections[j]->x,scale*list[i]->connections[j]->y),color, thickness, LINE_AA);
			
		}
	}
	return image;
}

Mat draw_closures_scaled(Mat input_image, vector<node**> closures, double scale, Scalar color = Scalar(0,0,255)){
	Mat image;
	input_image.copyTo(image);
	for (unsigned long long i = 0; i < closures.size(); ++i){
		line(image,Point(scale*closures[i][0]->x,scale*closures[i][0]->y),Point(scale*closures[i][1]->x,scale*closures[i][1]->y),color);
	}
	return image;
}

Mat draw_dots_scaled(Mat input_image, vector<node*> list, double scale, Scalar color = Scalar(255,0,0)){
	Mat image;
	input_image.copyTo(image);
	for (const auto& n:list){
		circle(image,Point(scale*n->x,scale*n->y),2,color);
	}
	return image;
}
Mat draw_line(vector<node*> loop, Mat image, double thickness, Scalar color = Scalar(0,0,255)){
	if (image.channels() == 1){
		cvtColor(image,image,COLOR_GRAY2BGR);
	}
	for (unsigned long long i = 0; i < loop.size() - 1; ++i){
		line(image,Point(loop[i]->x,loop[i]->y),Point(loop[i + 1]->x,loop[i + 1]->y),color,thickness);
	}
	return image;
}
bool linecomp(vector<node*> a, vector<node*> b){
	double d1 = line_length(a);
	double d2 = line_length(b);
	return (d1 < d2);
}
double lines_length(vector<vector<node*>> lines){
	double length = 0;
	for (const auto& l:lines){
		length += line_length(l);
	}
	return length;
}
Scalar HSVtRGB(double h, double s, double v){
	double r;
	double g;
	double b;
	uint8_t hi;
	double f;
	double p;
	double q;
	double t;
	hi = h / 60;
	f = h / 60.0 - hi;
	p = v * (1.0 - s);
	q = v * (1.0 - s * f);
	t = v * (1.0 - s * (1.0 - f));
	switch(hi) {
		case 0: b = p; g = t; r = v; break;
		case 1: b = p; g = v; r = q; break;
		case 2: b = t; g = v; r = p; break;
		case 3: b = v; g = q; r = p; break;
		case 4: b = v; g = p; r = t; break;
		case 5: b = q; g = p; r = v; break;
		case 6: b = p; g = t; r = v; break;
	}
	//cout << r << " " << g << " " << b << endl;
	return Scalar(b*255.0,g*255.0,r*255.0);
}

string replace_keywords(string);
void draw_lines(const Mat input_image, const vector<vector<node*>> lines, const string path, const double imagenumber = 10, const double thickness = 1){
	Mat image;
	input_image.copyTo(image);

	vector<vector<node*>> ls = lines;
	double total_length = lines_length(ls);
	sort(ls.begin(),ls.end(),linecomp);
	double acc_length = 0;
	const double image_line_length = (total_length / imagenumber);
	auto it = ls.begin();
	Scalar color;
	unsigned long long inum = 0;
	while (it != ls.end()){
		Mat temp;
		image.copyTo(temp);
		while (it != ls.end() && acc_length <= image_line_length){
			color = HSVtRGB(360*acc_length/image_line_length,1,1);
			acc_length += line_length(*it);
			temp = draw_line(*it,temp,thickness,color);
			++it;
		}
		++inum;
		imwrite(str_add_double(replace_keywords(path),inum)+".png",temp);
		acc_length = 0;
	}
}
#endif
