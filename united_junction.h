
#ifndef UNITED_JUNCTION_H
#define UNITED_JUNCTION_H UNITED_JUNCTION_H

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


class node;

class united_junction {
	public:
	vector<node*> nodes;
	vector<united_junction*>* list;
	vector<node**> outgoing_connections;
	double x;
	double y;
	unsigned long long dist;

	united_junction(node*,unsigned long long);
	~united_junction();
	
	void find_outgoing_connections();
	void determine_position();

};
	

#endif
