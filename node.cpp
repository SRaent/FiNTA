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
//		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RN && *it != mother){
		if (abs((*it)->x - x) <= RN && abs((*it)->y - y) <= RN && *it != mother){
			neighbors.push_back(*it);			
		}
	}
	
	list->push_back(this);
}

void node::procreate(cv::Mat img){
	std::vector<double>* fun = circlefun(img, x, y, 0, RV);
	double** smoothfun = gaussavgcircle(fun, STEPS, DEV);
	std::vector<unsigned long long> pks = findpks(smoothfun[1], STEPS);
	cv::Size s = img.size();
	node* child = NULL;
	bool too_close = false;
	for (std::vector<unsigned long long>::iterator it = pks.begin(); it != pks.end(); ++it){
		double xnew = x + (double)RS * cos(smoothfun[0][(*it)]);
		double ynew = y + (double)RS * sin(smoothfun[0][(*it)]);
		if (xnew > 0 && xnew < s.width && ynew > 0 && ynew < s.height){
			too_close = false;
			for (std::vector<node*>::iterator it = neighbors.begin();!too_close && it != neighbors.end(); ++it){
				if (pow(xnew - (*it)->x,2) + pow(ynew - (*it)->y,2) < pow(RF,2)){
					too_close = true;
				}
			}
			if(!too_close){
				child = new node(xnew, ynew, this, list);
				list->push_back(child);
				neighbors.push_back(child);
				connections.push_back(child);
			}
		}
	}
}

#endif