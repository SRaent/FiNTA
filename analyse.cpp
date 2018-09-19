#ifndef ANALYSE_CPP
#define ANALYSE_CPP ANALYSE_CPP

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


#define PI (double)3.1415926535897932384626433832795


//vector<vector<node*>*> find_loops(vector<node*>* list, vector<node**>* closures){
	
//}

vector<node*> find_loop(node** closure, vector<node*>* list){
	double x1 = (closure[0])->x;
	double x2 = (closure[1])->x;
	double y1 = (closure[0])->y;
	double y2 = (closure[1])->y;
	double x3r = ((closure[0])->connections.back())->x;
	double x3l = ((closure[0])->connections[0])->x;
	double y3r = ((closure[0])->connections.back())->y;
	double y3l = ((closure[0])->connections[0])->y;
	double xl = -(y2 - y1)/(2.0 * sqrt(pow(x2-x1,2)+pow(y2-y1,2))) - (y3l - y2)/(2.0 * sqrt(pow(x3l-x2,2)+pow(y3l-y2,2))) + x2;
	double yl = (x2 - x1)/(2.0 * sqrt(pow(x2-x1,2)+pow(y2-y1,2))) + (x3l - x2)/(2.0 * sqrt(pow(x3l-x2,2)+pow(y3l-y2,2))) + y2;
	double xr = (y2 - y1)/(2.0 * sqrt(pow(x2-x1,2)+pow(y2-y1,2))) + (y3r - y2)/(2.0 * sqrt(pow(x3r-x2,2)+pow(y3r-y2,2))) + x2;
	double yr = -(x2 - x1)/(2.0 * sqrt(pow(x2-x1,2)+pow(y2-y1,2))) - (x3r - x2)/(2.0 * sqrt(pow(x3r-x2,2)+pow(y3r-y2,2))) + y2;
	
	
	vector<node*> ret;
	return ret;
}



void only_loops(vector<node*> &list){
	
	bool buisy = true;
	while (buisy){
		buisy = false;
		for (unsigned long long i = 0; i < list.size(); ++i){
			if (list[i]->connections.size() <= 1){
				bool unfinished = true;
				for ( unsigned long long j = 0; j < list[i]->connections[0]->connections.size() && unfinished; ++j){
					if(list[i]->connections[0]->connections[j] == list[i]){
						list[i]->connections[0]->connections.erase(list[i]->connections[0]->connections.begin() + j);
						unfinished = false;
					}
				}
//				delete list[i];
				list.erase(list.begin() + i);
				cout << "r" << flush;
				buisy = true;
			}
		}
	}
}




/*
vector<node*> find_loop(node** closure, vector<node*>* list){
	vector<node> ret;
	for (auto i = (closure[0])->connections.begin(); i != (closure[0])->connections.back(); ++i){
		if (*i != closure[1]){
			ret = find_loop(closure, list, *i);
			ret.push_back(closure[0])
			return ret;
		}
	}
	
}

vector<node*> find_loop(node** closure, vector<node*>* list, node* n, node* p){

}
*/
#endif