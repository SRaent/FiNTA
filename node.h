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
	vector<node**>* closures;
	node* mother;
	Mat* img;
	Size s;
	bool procreated;
	double x;
	double y;
	double x_orig;
	double y_orig;
	
	
	node(double, double,vector<node*>*,Mat*);
	node(double, double,vector<node*>*,vector<node**>*,Mat*);
	node(double, double, node*);
	~node();
	void procreate(bool);
	void procreate_hessian(bool);
	void procreate_hessian_intersect(bool);
	bool connected(node*, unsigned long long);
	bool connected(node*, node*, unsigned long long);
	vector<node*> get_distant_connected(unsigned long long);
	vector<node*> get_distant_connected(node*, unsigned long long);
	node* get_straight_distant_connected(node*, unsigned long long);
	double score_connections(double,double,double);
	void wiggle(unsigned long long, double);
};

#endif
