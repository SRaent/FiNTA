#ifndef NODE_H
#define NODE_H NODE_H

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class node {
	public:
	std::vector<node*> neighbors;
	std::vector<node*> connections;
	std::vector<node*>* list;
	cv::Mat* img;
	cv::Size s;
	bool procreated;
	double x;
	double y;
	node(double, double,std::vector<node*>*,cv::Mat*);
	node(double, double, node*);
	void procreate(bool);
};

#endif