#ifndef NODE_CPP
#define NODE_CPP NODE_CPP

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "node.h"




node::node(double xinit, double yinit){
	procreated = 0;
	x = xinit;
	y = yinit;
}
node::node(double xinit, double yinit,node* mother,std::vector<node*>* listinit){
	procreated = 0;
	x = xinit;
	y = yinit;
	list = listinit;
	neighbors.push_back(mother);
	connections.push_back(mother);
	
	for (std::vector<node*>::iterator it = list->begin(); it != list->end(); ++it){
		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RF && *it != mother){
			neighbors.push_back(*it);			
		}
	}
	
	list->push_back(this);
}

void node::procreate(cv::Mat img){
	std::vector<double>* fun = circlefun(img, x, y, 0, RV);
	double** smoothfun = gaussavgcircle(fun, STEPS, DEV);
	std::vector<unsigned long long> pks = findpks(smoothfun[1], STEPS);
	
	for (std::vector<unsigned long long>::iterator it = pks.begin(); it != pks.end(); ++it){
		
	}
}

#endif