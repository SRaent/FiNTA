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
	vector<node*> neighbors;
	vector<node*> connections;
	vector<node*>* list;
	node* mother;
	Mat* img;
	Size s;
	bool procreated;
	double x;
	double y;
	node(double, double,vector<node*>*,Mat*);
	node(double, double, node*);
	void procreate(bool);
	void procreate_hessian(bool);
	bool connected(node*, unsigned long long);
	bool connected(node*, node*, unsigned long long);
};

#endif