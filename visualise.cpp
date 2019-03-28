#ifndef VISUALISE_CPP
#define VISUALISE_CPP VISUALISE_CPP

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
		sprintf(filename,"%sloop%d.tif",folder,l);
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
#endif