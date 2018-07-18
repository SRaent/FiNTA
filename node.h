#ifndef NODE_H
#define NODE_H NODE_H

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string.h>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class node {
	std::vector<node*> neighbors;
	std::vector<node*> connections;
	std::vector<node*>* list;
	node(unsigned long long, unsigned long long);
	node(unsigned long long, unsigned long long, node*, std::vector<node*>*);
};

#endif