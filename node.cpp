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
#include "analyse.cpp"




node::node(double xinit, double yinit, std::vector<node*>* listinit, cv::Mat* initimg){
	list = listinit;
	img = initimg;
	s = img->size();
	procreated = 0;
	x = xinit;
	y = yinit;
	list->push_back(this);
	closures = NULL;
}

node::node(double xinit, double yinit, std::vector<node*>* listinit, std::vector<node**>* closuresinit, cv::Mat* initimg){
	list = listinit;
	img = initimg;
	closures = closuresinit;
	s = img->size();
	procreated = 0;
	x = xinit;
	y = yinit;
	list->push_back(this);
}

node::node(double xinit, double yinit,node* mother_init){
	mother = mother_init;
	procreated = 0;
	x = xinit;
	y = yinit;
	list = mother->list;
	img = mother->img;
	closures = mother->closures;
	s = img->size();
	connections.push_back(mother);
	//mother->neighbors.push_back(this);
	mother->connections.push_back(this);
	list->push_back(this);
}

node::~node(){
}

void node::procreate(bool free = 1){
	procreated = 1;
	
	for (std::vector<node*>::iterator it = list->begin(); it != list->end(); ++it){
//		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RN && *it != mother){
		if (abs((*it)->x - x) <= RN && abs((*it)->y - y) <= RN){
			neighbors.push_back(*it);			
		}
	}
	
	std::vector<double>* fun = circlefun(img, x, y, RM, RV);
	double** smoothfun = gaussavgcircle(fun, STEPS, DEV, free);
	std::vector<unsigned long long> pks = findpks(smoothfun[1], STEPS);
	node* child = NULL;
	bool too_close = false;
	bool closable = false;
	double min_dist = numeric_limits<double>::infinity();
	bool unconned = true;
	double dist = 0;
	for (std::vector<unsigned long long>::iterator it = pks.begin(); it != pks.end(); ++it){
		double xnew = x + (double)RS * cos(smoothfun[0][(*it)]);
		double ynew = y + (double)RS * sin(smoothfun[0][(*it)]);
		double xtest = x + (double)RT * cos(smoothfun[0][(*it)]);
		double ytest = y + (double)RT * sin(smoothfun[0][(*it)]);
		if (xnew > 0 && xnew < s.width && ynew > 0 && ynew < s.height){
			closable = false;
			too_close = false;
			min_dist = numeric_limits<double>::infinity();
			for (unsigned long long j = 0; j < neighbors.size(); ++j){
				dist = pow(xnew - (neighbors[j])->x,2) + pow(ynew - (neighbors[j])->y,2);
				if (dist < pow(RF,2)){
					too_close = true;
					if(dist < min_dist && !connected(neighbors[j],ML)){
						child = neighbors[j];
						min_dist = dist;
						closable = true;
					}
				}
			}
			
			if(closable || !too_close) {
				vector<double>* lfun = linefun(img, x, y, xtest, ytest, LT);
				
				if(gaussavgoverthresh(lfun, LT, LS, LD, CT, free)){
					if (!too_close){
						new node(xnew, ynew, this);
		//				std::cout << xnew << " " << ynew << std::endl;
					}
					else{
//						cout << "C" << flush;
						connections.push_back(child);
						child->connections.push_back(this);
						closures->push_back(new node*[2]);
						(closures->back())[0] = this;
						(closures->back())[1] = child;
					}
				}
			}
		}
	}
	if (free){
		delete[] smoothfun[0];
		delete[] smoothfun[1];
	}
}


void node::procreate_hessian(bool free = 1){
	procreated = 1;
	
	for (std::vector<node*>::iterator it = list->begin(); it != list->end(); ++it){
//		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RN && *it != mother){
		if (abs((*it)->x - x) <= RN && abs((*it)->y - y) <= RN){
			neighbors.push_back(*it);			
		}
	}
	
	std::vector<double>* fun = circlefun_hessian(img, x, y, RM, RV);
	double** smoothfun = gaussavgcircle_MT(fun, STEPS, DEV, free);
	std::vector<unsigned long long> pks = findpks_thresh(smoothfun[1], STEPS,TH);
	node* child = NULL;
	bool too_close = false;
	bool closable = false;
	double dist = 0;
	double min_dist = numeric_limits<double>::infinity();
	for (std::vector<unsigned long long>::iterator it = pks.begin(); it != pks.end(); ++it){
		double xnew = x + (double)RS * cos(smoothfun[0][(*it)]);
		double ynew = y + (double)RS * sin(smoothfun[0][(*it)]);
		if (xnew > 0 && xnew < s.width && ynew > 0 && ynew < s.height){
			too_close = false;
			closable = false;
			min_dist = numeric_limits<double>::infinity();
			for (unsigned long long j = 0; j < neighbors.size(); ++j){
				dist = pow(xnew - (neighbors[j])->x,2) + pow(ynew - (neighbors[j])->y,2);
				if (dist < pow(RF,2)){
					too_close = true;
					if(dist < min_dist && !connected(neighbors[j],ML)){
						child = neighbors[j];
						min_dist = dist;
						closable = true;
					}
				}
			}
			if (!too_close){
				new node(xnew, ynew, this);
//				std::cout << xnew << " " << ynew << std::endl;
			}
			else if(closable){
				//cout << "c" << flush;
				node* inbetween = new node((x + child->x)/2.0, (y + child->y)/2.0, this);
				inbetween->procreated = true;
				inbetween->connections.push_back(child);
				child->connections.push_back(inbetween);
				node** closure = new node*[2];
				closures->push_back(closure);
				closure[0] = this;
				closure[1] = inbetween;
			}
		}
	}
	if (free){
		delete[] smoothfun[0];
		delete[] smoothfun[1];
	}
}



//this function is a piece of shit and does not work at all, do not use it without extensive debugging!!!!
//i know why it does not work! there is no check, weather the intersection is in the 2nd line, one would have to check weather crosspar[3] is between 0 and 1 before saying the 2 lines intersect. not gonna fix it tho, coz this function is a POS!!!
void node::procreate_hessian_intersect(bool free = 1){
	procreated = 1;
	
	for (std::vector<node*>::iterator it = list->begin(); it != list->end(); ++it){
//		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RN && *it != mother){
		if (abs((*it)->x - x) <= RN && abs((*it)->y - y) <= RN){
			neighbors.push_back(*it);			
		}
	}
	
	std::vector<double>* fun = circlefun_hessian(img, x, y, RM, RV);
	double** smoothfun = gaussavgcircle_MT(fun, STEPS, DEV, free);
	std::vector<unsigned long long> pks = findpks_thresh(smoothfun[1], STEPS,TH);
	
	//check weather already existing connections are angularly to close to the planned new ones. 2 new connections can be closer togehter.
	for (unsigned long long i = 0; i < pks.size(); ++i){
		for (unsigned long long j = 0; j < connections.size(); ++j){
			if (rel_half_angle(connections[j]->y- x,connections[j]->y - y, smoothfun[0][pks[i]]) < FA ){
				pks.erase(pks.begin() + i);
				--i;
			}
		}
	}
	
	bool clipping = false;
	bool closable = false;
	double alpha = 0;
	double min_alpha = 1.0;
	double crosspar[4];
	node* nconn;
	node* closest_clip1;
	node* closest_clip2;
	double xclip;
	double yclip;
	node* new_clip_node;
	for (std::vector<unsigned long long>::iterator it = pks.begin(); it != pks.end(); ++it){
		double xnew = x + (double)RS * cos(smoothfun[0][(*it)]);
		double ynew = y + (double)RS * sin(smoothfun[0][(*it)]);
		if (xnew > 0 && xnew < s.width && ynew > 0 && ynew < s.height){
			clipping = false;
			closable = false;
			min_alpha = 1.0;
			for (unsigned long long j = 0; j < neighbors.size(); ++j){
				for (unsigned long long k = 0; k < neighbors[j]->connections.size(); ++k){
					nconn = neighbors[j]->connections[k];
					find_intersect(crosspar,x,y,xnew,ynew,neighbors[j]->x,neighbors[j]->y,nconn->x, nconn->y);
					if (crosspar[2] < min_alpha && crosspar[2] > 0.0){
						clipping = true;
						min_alpha = crosspar[2];
						//PRINT(min_alpha);
						if (!connected(neighbors[j],ML) && !connected(nconn,ML)){
							closable = true;
							xclip = crosspar[0];
							yclip = crosspar[1];
							closest_clip1 = neighbors[j];
							closest_clip2 = nconn;
						}
					}
				}
			}
			if (!clipping){
				new node(xnew, ynew, this);
//				std::cout << xnew << " " << ynew << std::endl;
			}
			else if(closable){
				cout << "c" << flush;
				new_clip_node = new node(xclip, yclip, this);
				new_clip_node->connections.push_back(closest_clip1);
				new_clip_node->connections.push_back(closest_clip2);
				closest_clip1->connections.erase(closest_clip1->connections.begin() + find_connection_index(closest_clip1,closest_clip2));
				closest_clip2->connections.erase(closest_clip2->connections.begin() + find_connection_index(closest_clip2,closest_clip1));
				closest_clip1->connections.push_back(new_clip_node);
				closest_clip2->connections.push_back(new_clip_node);
				node** closure = new node*[2];
				closures->push_back(closure);
				closure[0] = this;
				closure[1] = new_clip_node;
			}
		}
	}
	if (free){
		delete[] smoothfun[0];
		delete[] smoothfun[1];
	}
}



bool node::connected(node* n, unsigned long long l){
	bool ret = false;
	for (unsigned long long i = 0; i < connections.size() && !ret; ++i){
		if (connections[i] == n){
			ret = true;
//			cout << l << " " << flush;
		}
		else if (l > 0 && !ret){
			ret = connections[i]->connected(n,this,(l - 1));
		}
	}
	return ret;
}

bool node::connected(node* n, node* origin, unsigned long long l){
	bool ret = false;
	for (unsigned long long i = 0; i < connections.size() && !ret; ++i){
		if (connections[i] != origin){
			if (connections[i] == n){
				ret =  true;
//				cout << l << " " << flush;
			}
			else if (l > 0 && !ret){
				ret = connections[i]->connected(n,this,(l - 1));
			}
		}
	}
	return ret;
}

vector<node*> node::get_distant_connected(node* origin, unsigned long long dist){
	vector<node*> dc;
	if (dist == 1){
		for (unsigned long long i = 0; i < connections.size(); ++i){
			if (connections[i] != origin){
				dc.push_back(connections[i]);
			}
		}
	}
	else {
		vector<node*> dcr;
		for (unsigned long long i = 0; i < connections.size(); ++i){
			if (connections[i] != origin){
				 dcr = connections[i]->get_distant_connected(this, dist - 1);
				 dc.insert(dc.end(),dcr.begin(),dcr.end());
			}
		}
	}
	return dc;
	
}

vector<node*> node::get_distant_connected(unsigned long long dist){
	vector<node*> dc;
	if (dist == 1){
		dc = connections;
	}
	else {
		vector<node*> dcr;
		for (unsigned long long i = 0; i < connections.size(); ++i){
			 dcr = connections[i]->get_distant_connected(this, dist - 1);
			 dc.insert(dc.end(),dcr.begin(),dcr.end());
		}
	}
	return dc;
}

node* node::get_straight_distant_connected(node* origin, unsigned long long dist){
	node* dc;
	if (dist == 0){
		dc = this;
	}
	else {
		if (connections.size() < 2){
			dc = this;
		}
		else if (connections.size() == 2){
			if (connections[0] == origin){
				dc = connections[1]->get_straight_distant_connected(this, dist - 1);
			}
			else if(connections[1] == origin){
				dc = connections[0]->get_straight_distant_connected(this, dist - 1);
			}
			else {cout << "something fucky goin on, you try to find the streightest connection between 2 non connected nodes" << endl;}
		}
		else {
			double max_angle = 0;
			double angle = 0;
			double dx1 = origin->x - x;
			double dy1 = origin->y - y;
			double dx2 = 0;
			double dy2 = 0;
			for (unsigned long long i = 0; i < connections.size(); ++i){
				if (connections[i] != origin){
					dx2 = connections[i]->x - x;
					dy2 = connections[i]->y - y;
					
					angle = rel_half_angle(dx1,dy1,dx2,dy2);
					
					if (angle > max_angle){
						max_angle = angle;
						dc = connections[i];
					}
				}
			}
			dc = dc->get_straight_distant_connected(this, dist - 1);
		}
	}
	return dc;
	
}
#endif