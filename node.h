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
	bool procreated;
	double x;
	double y;
	node(double, double);
	node(double, double, node*, std::vector<node*>*);
	void procreate(cv::Mat);
};

#endif