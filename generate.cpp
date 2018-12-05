#ifndef GENERATE_CPP
#define GENERATE_CPP GENERATE_CPP

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

bool draw_infinite_line(Mat &img,double x1,double y1,double x2,double y2, Scalar color = 255, unsigned long long thickness = 1){
	Size s = img.size();
	bool intersect = false;
	double crosspar1[4];
	double crosspar2[4];
	double linepar[4];
	bool found_corss;
	double* crosspar = crosspar1;
	//s.widht
	//s.hight
	// this part has a lot of exeptions you dont imidiatly think about and the <= and < are very important to destinguish!!!
	find_intersect(crosspar, x1, y1, x2, y2, 0.0, 0.0, s.width, 0.0);
	if (crosspar[3] <= 1.0 && crosspar[3] > 0.0){crosspar = crosspar2; intersect = true;}
	find_intersect(crosspar, x1, y1, x2, y2, 0.0, 0.0, 0.0, s.height);
	if (crosspar[3] < 1.0 && crosspar[3] >= 0.0){
		if (intersect){
			goto intersects_found;
		}
		else {
			crosspar = crosspar2;
			intersect = true;
		}
	}
	find_intersect(crosspar, x1, y1, x2, y2, 0.0, s.height, s.width, s.height);
	if (crosspar[3] < 1.0 && crosspar[3] >= 0.0){
		if (intersect){
			goto intersects_found;
		}
		else {
			crosspar = crosspar2;
			intersect = true;
		}
	}
	find_intersect(crosspar, x1, y1, x2, y2, s.width, 0.0, s.width, s.height);
	if (crosspar[3] <= 1.0 && crosspar[3] > 0.0 && !intersect){
		cout << "something fucky goin on in draw_infinite_line" << endl;
	}
	if (!(crosspar[3] <= 1.0 && crosspar[3] > 0.0) && intersect){
		cout << "something fucky goin on in draw_infinite_line" << endl;
		//PRINT(intersect)
	}
intersects_found:
	if (!intersect){
	/*
		PRINT(crosspar1[0])
		PRINT(crosspar1[1])
		PRINT(crosspar1[2])
		PRINT(crosspar1[3])
		PRINT(crosspar2[0])
		PRINT(crosspar2[1])
		PRINT(crosspar2[2])
		PRINT(crosspar2[3])
		*/
		return intersect;
	}
	
	line(img, Point(crosspar1[0],crosspar1[1]), Point(crosspar2[0],crosspar2[1]), color, thickness);
	/*
	PRINT(crosspar1[0])
	PRINT(crosspar1[1])
	PRINT(crosspar2[0])
	PRINT(crosspar2[1])
	*/
	return intersect;
}


void gen_streight_lines(Mat &img, unsigned long long linenumber1, double angle1, unsigned long long linethickness = 1, Scalar linecolor = 255){
	
	Size s = img.size();
	
	unsigned long long xsize = s.width;
	unsigned long long ysize = s.height;
	
	
	while (angle1 < 0){
		angle1 += PI;
	}
	while (angle1 >= PI){
		angle1 -= PI;
	}
	
	double dx1 = cos(angle1);
	double dy1 = sin(angle1);
	
	
	double spawnpar1[4];
	find_intersect(spawnpar1, ((double)xsize)/2.0, ((double)ysize)/2.0, ((double)xsize)/2.0 + dy1, ((double)ysize)/2.0 - dx1, 0.0, 0.0, dx1, dy1);
	if(!(spawnpar1[0] < 0 || spawnpar1[0] > xsize || spawnpar1[1] < 0 || spawnpar1[1] > ysize)){
		find_intersect(spawnpar1, ((double)xsize)/2.0, ((double)ysize)/2.0, ((double)xsize)/2.0 + dy1, ((double)ysize)/2.0 - dx1, 0.0, ysize, dx1, ysize + dy1);
		cout << "2nd intersect" << endl;
	}
	
	//spawnpar1[2] = xsize - spawnpar1[0];
	//spawnpar1[3] = ysize - spawnpar1[1];
	//now spawnpar1 containes all the fucking endpoints, where the last possible visible line can be spawned in the direction of angle1
	
	double dxs1 = (xsize - 2.0 * spawnpar1[0])/(linenumber1 + 1);
	double dys1 = (ysize - 2.0 * spawnpar1[1])/(linenumber1 + 1);
	
	for (unsigned long long i = 1; i < linenumber1 + 1; ++i){
		draw_infinite_line(img, spawnpar1[0] + i * dxs1, spawnpar1[1] + i * dys1, spawnpar1[0] + i * dxs1 + dx1, spawnpar1[1] + i * dys1 + dy1, linecolor, linethickness);
	}
	
}


#endif