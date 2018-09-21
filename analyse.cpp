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



unsigned long long find_connection_index(node* n, node* connection){
	for (unsigned long long i = 0; i < n->connections.size(); ++i){
		if(n->connections[i] == connection){
			return i;
		}
	}
	cout << "-------------------------- SOMTHING WENT  TERRIBLY WRONG --------------------------" << endl;
	return 0 - 1; //this is supposed to cause some other error while running (gr8 error handeling i know, but i am a noob) when the 2 nodes are not connected. (underflow of unsigned long long will happen, propaply fucking up something somewhere very strange. (i speak from experience))
}

// sign = 1: right way turn. sign  = -1 left way turn
vector<node*> find_loop(node* prev, node* n, node* start, long long sign){
	vector<node*> ret;
//	cout << n->x << " " << n->y << " " << n << " " << prev << " " << start << " ";
	if (n == start){
		ret.push_back(n);
		return ret;
	}
	else{
		double prevang = atan2(n->x - prev->x, n->y - prev->y);
		double minang = 2 * PI;
		double ang = 0;
		unsigned long long index = 0;
		
		for (unsigned long long i = 0; i < n->connections.size(); ++i){
			ang = sign * (atan2(n->x - n->connections[i]->x, n->y - n->connections[i]->y) - prevang);
			while (ang < 0){
				ang += 2 * PI;
			}
			while (ang > 2 * PI){
				ang -= 2 * PI;
			}
			if (ang < minang && ang != 0.0){
				minang = ang;
				index = i;
			}
			
		}
//		cout << find_connection_index(n, prev) << " " << index << " " << n->connections.size() << endl;
		ret = find_loop(n, n->connections[index], start, sign);
		ret.push_back(n);
	}
	return ret;
}

vector<node*> find_loop(node** closure, long long sign){
	return find_loop(closure[0],closure[1],closure[0],sign);
}

double loop_checksum(vector<node*> loop){
	double ret = 0;
	for (unsigned long long i = 0; i < loop.size(); ++i){
		ret += loop[i]->x + loop[i]->y;
	}
	return ret;
}

vector<vector<node*>> find_loops(vector<node**> closures){
	
	vector<vector<node*>> ret;
	
	for (unsigned long long i = 0; i < closures.size(); ++i){
		ret.push_back(find_loop(closures[i], 1));
		ret.push_back(find_loop(closures[i], -1));
	}
	
	double check = 0;
		
	for (unsigned long long i = 0; i < ret.size() - 1; ++i){
		check = loop_checksum(ret[i]);
		for (unsigned long long j = i + 1; j < ret.size(); ++j){
			if(abs(loop_checksum(ret[j]) - check) < 0.000001){
//				PRINT(abs(loop_checksum(ret[j]) - check))
				ret.erase(ret.begin() + j);
				--j;
//				cout << "checksum match " << i + 1 << endl;
			}
		}
	}
	
	return ret;
}



void only_loops(vector<node*> &list){
	
	bool buisy = true;
	while (buisy){
		
		buisy = false;
		for (unsigned long long i = 0; i < list.size(); ++i){
			if (list[i]->connections.size() <= 1){
				bool unfinished = true;
				if (list[i]->connections.size() == 1){
					list[i]->connections[0]->connections.erase(list[i]->connections[0]->connections.begin() + find_connection_index(list[i]->connections[0], list[i]));
				}
				delete (list[i]);
				list.erase(list.begin() + i);
//				cout << "r";
				buisy = true;
			}
		}
	}
}


double loop_area(vector<node*> loop){
	double area = 0.0;
	unsigned long long j = 0;
	for (unsigned long long i = 0; i < loop.size(); ++i){
		j = (i + 1);
		if (j >= loop.size()){
			j -= loop.size();
		}
		area += (loop[i]->x) * (loop[j]->y);
		area -= (loop[i]->y) * (loop[j]->x);
	}
	return abs(area/2.0);
}


double loop_length(vector<node*> loop){
	double length = 0.0;
	unsigned long long j = 0;
	for (unsigned long long i = 0; i < loop.size(); ++i){
		j = (i + 1);
		if (j >= loop.size()){
			j -= loop.size();
		}
		length += sqrt(pow(loop[i]->x - loop[j]->x,2) + pow(loop[i]->y - loop[j]->y,2));
	}
	return length;
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