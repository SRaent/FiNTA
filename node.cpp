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




node::node(double xinit, double yinit,vector<node*>* listinit, Mat* initimg){
	list = listinit;
	img = initimg;
	s = img->size();
	procreated = 0;
	x = xinit;
	y = yinit;
	list->push_back(this);
}
node::node(double xinit, double yinit,node* mother){
	procreated = 0;
	x = xinit;
	y = yinit;
	list = mother->list;
	img = mother->img;
	s = img->size();
	neighbors.push_back(mother);
	connections.push_back(mother);
	
	for (vector<node*>::iterator it = list->begin(); it != list->end(); ++it){
//		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RN && *it != mother){
		if (abs((*it)->x - x) <= RN && abs((*it)->y - y) <= RN && *it != mother){
			neighbors.push_back(*it);			
		}
	}
	
	list->push_back(this);
}

void node::procreate(bool free = 1){
	procreated = 1;
	vector<double>* fun = circlefun(*img, x, y, RM, RV);
	double** smoothfun = gaussavgcircle(fun, STEPS, DEV);
	vector<unsigned long long> pks = findpks(smoothfun[1], STEPS);
	node* child = NULL;
	bool too_close = false;
	for (vector<unsigned long long>::iterator it = pks.begin(); it != pks.end(); ++it){
		double xnew = x + (double)RS * cos(smoothfun[0][(*it)]);
		double ynew = y + (double)RS * sin(smoothfun[0][(*it)]);
		if (xnew > 0 && xnew < s.width && ynew > 0 && ynew < s.height){
			too_close = false;
			for (vector<node*>::iterator it = neighbors.begin();!too_close && it != neighbors.end(); ++it){
				if (pow(xnew - (*it)->x,2) + pow(ynew - (*it)->y,2) < pow(RF,2)){
					too_close = true;
				}
			}
			if(!too_close){
				child = new node(xnew, ynew, this);
				list->push_back(child);
				neighbors.push_back(child);
				connections.push_back(child);
//				cout << xnew << " " << ynew << endl;
			}
		}
	}
	if (free){
		delete[] smoothfun[0];
		delete[] smoothfun[1];
	}
}

#endif