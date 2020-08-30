#ifndef NODE_CPP
#define NODE_CPP NODE_CPP

#include <limits>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


#include "opencv2/core.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "node.h"
#include "analyse.cpp"
#include "userinterface.cpp"

using namespace std;
using namespace cv;


bool went_down = false;

node::node(double xinit, double yinit, std::vector<node*>* listinit, cv::Mat* initimg){
	list = listinit;
	img = initimg;
	s = img->size();
	procreated = 0;
	x = xinit;
	y = yinit;
	list->push_back(this);
	closures = NULL;
	j = NULL;
	/*
	if (abs(332.5 - x) < 1 && abs(657.0 - y) < 1){
		cout << "ladies and gentleman" << endl;
		cout << "we got him" << endl;
	}
	*/
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
	j = NULL;
	/*
	if (abs(332.5 - x) < 1 && abs(657.0 - y) < 1){
		cout << "ladies and gentleman" << endl;
		cout << "we got him" << endl;
	}
	*/
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
	j = NULL;
	/*
	if (abs(322.5 - x) < 1 && abs(657.0 - y) < 1){
		cout << "ladies and gentleman" << endl;
		PRINT(this)
		PRINT(x)
		PRINT(y)
		cout << "we got him" << endl;
	}
	for (const auto& n:*list){
		if (n != this && abs(n->x - x) < 0.01 && abs(n->y - y) < 0.01){
			cout << "SHIT WENT DOWN BOIS" << endl;
		}
	}
	*/
}

node::~node(){
	/*
	for (const auto& n:*list){
		if (n != this){
			if (abs(n->x - x) < 0.01 && abs(n->y - y) < 0.01){
				cout << "SHIT GOT FIXED BOIS, we good" << endl;
			}
		}
	}
	*/
	//cout << "deleting node" << endl;
	for (unsigned long long i = 0; i < connections.size(); ++i){
		//cout << "erasing connection " << i << " of " << connections.size() << endl;
		(connections[i]->connections).erase((connections[i]->connections).begin() + find_connection_index((connections[i]), this));
	}
	//cout << "erased connections" << endl;
	for (unsigned long long j = 0; j < closures->size(); ++j){
		if (this == (*closures)[j][0] || this == (*closures)[j][1]){
			//njow all trjaces mjust bj djelijtjed!!! just ljke staljns opponjents!!
			delete[] (*closures)[j];
			
			//cout << "about to erase closure" << endl;
			closures->erase(closures->begin() + j);
			--j;
		}
	}
	list->erase(list->begin() + find_node_list_position(this, *list));
	//cout << "erased node" << endl;
}
/* OBSOLETE
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
*/


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
				neighbors.push_back(new node(xnew, ynew, this));
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
				neighbors.push_back(inbetween);
			}
		}
	}
	if (free){
		delete[] smoothfun[0];
		delete[] smoothfun[1];
	}
}

bool node::connect(node* con){
	double xpos = (x + con->x)/2.0;
	double ypos = (y + con->y)/2.0;
	vector<node*> neigh;
	for (std::vector<node*>::iterator it = list->begin(); it != list->end(); ++it){
//		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RN && *it != mother){
		if ((*it) != this && (*it) != con && abs((*it)->x - xpos) < RF && abs((*it)->y - ypos) < RF){
			neigh.push_back(*it);			
		}
	}
	for (const auto& n:neigh){
		if (sqr(n->x-xpos)+sqr(n->y-ypos) < sqr(RF)){
			//cout << this << " " << con << " " << n << endl;
			return false;
		}
	}
	
	node* inbetween = new node(xpos,ypos, this);
	inbetween->procreated = true;
	inbetween->connections.push_back(con);
	con->connections.push_back(inbetween);
	node** closure = new node*[2];
	closures->push_back(closure);
	closure[0] = this;
	closure[1] = inbetween;
	return true;
}
bool node::connect(node* con, node*& inbet){
	double xpos = (x + con->x)/2.0;
	double ypos = (y + con->y)/2.0;
	vector<node*> neigh;
	for (std::vector<node*>::iterator it = list->begin(); it != list->end(); ++it){
//		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RN && *it != mother){
		if ((*it) != this && (*it) != con && abs((*it)->x - xpos) < RF && abs((*it)->y - ypos) < RF){
			neigh.push_back(*it);			
		}
	}
	for (const auto& n:neigh){
		if (sqr(n->x-xpos)+sqr(n->y-ypos) < sqr(RF)){
			return false;
		}
	}
	
	node* inbetween = new node(xpos,ypos, this);
	inbetween->procreated = true;
	inbetween->connections.push_back(con);
	con->connections.push_back(inbetween);
	node** closure = new node*[2];
	closures->push_back(closure);
	closure[0] = this;
	closure[1] = inbetween;
	inbet = inbetween;
	return true;
}

void node::disconnect(node* con){
	bool f1 = false;
	bool f2 = false;
	for (auto it = connections.begin(); it != connections.end(); ++it){
		if (*it == con){
			connections.erase(it);
			f1 = true;
			break;
		}
	}
	for (auto it = con->connections.begin(); it != con->connections.end(); ++it){
		if (*it == this){
			f2 = true;
			con->connections.erase(it);
			break;
		}
	}
	for (auto it = closures->begin(); it != closures->end(); ++it){
		if ((*it)[0] == this && (*it)[1] == con){
			//cout << "erased_closure" << endl;
			delete[] (*it);
			closures->erase(it);
			break;
		}
		if ((*it)[1] == this && (*it)[0] == con){
			//cout << "erased_closure" << endl;
			delete[] (*it);
			closures->erase(it);
			break;
		}
	}
	if (!f1 && !f2) {cout << "trying to disconnect non connected nodes" << endl;}
	else if (!f1 || !f2) {cout << "node_structure_fucked" << endl;}
}

vector<node*> node::find_worst_sect(const vector<vector<node*>>& sections){
	double thickness = 2.0*SG;
	if (sect_thickness_set){
		thickness = sect_thickness;
	}
	vector<node*> worst_section;
	double worst_score = numeric_limits<double>::infinity();
	double score;

	for (const auto& s:sections){
		//if (!is_line(s) || s.front()->connections.size() < 3 || s.back()->connections.size() < 3){cout << "sectioning gone wrong" << endl;}
		score = score_line(s,thickness);
		if( score < worst_score){
			worst_score = score;
			worst_section = s;
		}
	}
	return worst_section;
}

vector<vector<node*>> split_loop_junc(vector<node*>);

bool node::repair_tight_closure(node* close, unsigned long long min_loop_length, vector<node*>& deleted){
	while (connected(close,min_loop_length)){
		went_down = false;
		vector<node*> loop = connecting_line(close);
		node* inbetween;
		if (!connect(close,inbetween)){
			return false;
		}
		loop.push_back(inbetween);

		vector<vector<node*>> sections = split_loop_junc(loop);
		if (sections.size() <= 1){
			delete inbetween;
			return false;
		}
		vector<node*> worst_section = find_worst_sect(sections);

		if (worst_section.size() == 2){
			worst_section.front()->disconnect(worst_section.back());
		}
		else {
			bool harakiri = false;
			bool del_inbetween = false;
			for (unsigned long long i = 1; i < worst_section.size() - 1; ++i){
				if (worst_section[i] != this){
					if (worst_section[i] == inbetween){del_inbetween = true;}
					deleted.push_back(worst_section[i]);
					delete worst_section[i];
				}
				else {harakiri = true;}
			}
			if (harakiri) {return true;}
			else if (del_inbetween) {return false;} //the else is only for code readability
		}
		delete inbetween;
	}
	connect(close);
	return false;
}
/*
bool node::repair_tight_closure(node* close, unsigned long long min_loop_length, vector<node*>& deleted){
	node* inbetween = connect(close);

	vector<node*> loop = find_loop(closure,1);
	if (!is_loop(loop)){
		cout << "loop is not a loop" << endl;
	}

	vector<vector<node*>> sections = split_loop_junc(loop);

	if (sections.size() <= 1){
		delete inbetween;
		return false;
	}

	vector<node*> worst_section = find_worst_sect(sections);

	if( worst_section.size() == 2){
		worst_section.front()->disconnect(worst_section.back());
	}
	else if (worst_section.size() > 2){
		for (unsigned long long i = 1; i < worst_section.size() - 1; ++i){
			if(worst_section[i] != this){
				delete worst_section[i];
			}
			else {return true;}
		}
	}
	else {cout << "the sectioning function is fucked" << endl;}

	return false;
}
*/

void node::procreate_hessian_rate(bool free = 1){
	procreated = 1;
	
	for (std::vector<node*>::iterator it = list->begin(); it != list->end(); ++it){
//		if (sqrt(pow((*it)->x - x,2) + pow((*it)->y - y,2)) <= RN && *it != mother){
		if (abs((*it)->x - x) <= RN && abs((*it)->y - y) <= RN && *it != this){
			neighbors.push_back(*it);			
		}
	}

	vector<node*> tight_closures;
	vector<node*> deleted;
	
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
			child = NULL;
			for (unsigned long long j = 0; j < neighbors.size(); ++j){
				dist = pow(xnew - (neighbors[j])->x,2) + pow(ynew - (neighbors[j])->y,2);
				if (dist < pow(RF,2)){
					too_close = true;
					if(dist < min_dist && !neighbors[j]->is_in(connections)){
						closable = false;
						child = neighbors[j];
						min_dist = dist;
						if (!connected(neighbors[j],ML)){
							closable = true;
						}
					}
				}
			}
			if (!too_close){
				neighbors.push_back(new node(xnew, ynew, this));
//				std::cout << xnew << " " << ynew << std::endl;
			}
			else if(child != NULL){
				if (closable){
					node* inbetween;
					if (connect(child,inbetween)){
						//cout << "c" << flush;
						neighbors.push_back(inbetween);
					}
					//else {cout <<"n"<<flush;}
					deleted.push_back(child);
				}
				else if (!child->is_in(tight_closures)) {
					tight_closures.push_back(child);
				}
			}
		}
	}
	bool harakiri = false;
	//if (tight_closures.size() > 1)
		//PRINT(tight_closures.size())
	for (const auto& c:tight_closures){
		if (!c->is_in(deleted) && !harakiri){
			harakiri = repair_tight_closure(c,ML,deleted);
		}
	}
	if (free){
		delete[] smoothfun[0];
		delete[] smoothfun[1];
	}
	if (harakiri){
		// it was an honor serving with you
		//cout << "AXIOS" << endl;
		delete this;
	}
}



//this function is a piece of shit and does not work at all, do not use it without extensive debugging!!!!
//i know why it does not work! there is no check, weather the intersection is in the 2nd line, one would have to check weather crosspar[3] is between 0 and 1 before saying the 2 lines intersect. not gonna fix it tho, coz this function is a POS!!!
/*
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
*/

vector<node*> node::connecting_line(node* con){
	vector<node*> ret;
	vector<node*> processing;
	vector<unsigned long long> prev_n_ind;

	processing.push_back(this);

	unsigned long long processed = 0;
	bool searching = true;

	//cout << "bout to branch out" << endl;

	while (processed < processing.size() && searching){
		for (const auto& n:processing[processed]->connections){
			if(n == con){
				searching = false;
				//prev_n_ind.push_back(processed);
			}
			else if(!n->is_in(processing) && searching){
				processing.push_back(n);
				prev_n_ind.push_back(processed);
			}
		}
		++processed;
	}
	--processed;
	//cout << "branched out" << endl;

	if (searching == true) {cout << "no connecting line" << endl;return ret;}

	ret.push_back(con);
	//processed = prev_n_ind.back(); //Not necessary

	//PRINT(processing.size())
	//PRINT(prev_n_ind.size())
	while(processed > 0){
		//PRINT(processed)
		ret.push_back(processing[processed]);
		processed = prev_n_ind[processed - 1];
	}

	ret.push_back(this);
	//PRINT(ret.size())
	
	if (!is_line(ret)){cout << "connecting_line function is fucked" << endl;}

	return ret;
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

vector<node*> node::get_all_distant_connected(node* origin, unsigned long long dist){
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
				dcr = connections[i]->get_all_distant_connected(this, dist - 1);
				dc.insert(dc.end(),dcr.begin(),dcr.end());
			}
		}
	}
	dc.push_back(this);
	
	return dc;
	
}

vector<node*> node::get_all_distant_connected(vector<node*>& dc, unsigned long long dist){
	dc.push_back(this);
	if (dist != 0) {
		for (unsigned long long i = 0; i < connections.size(); ++i){
			if (!connections[i]->is_in(dc)){
				connections[i]->get_all_distant_connected(dc, dist - 1);
			}
		}
	}
	
	return dc;
	
}
vector<node*> node::get_all_distant_connected(unsigned long long dist){
	vector<node*> dc;
	dc.push_back(this);
	if (dist != 0){
		for (unsigned long long i = 0; i < connections.size(); ++i){
			connections[i]->get_all_distant_connected(dc, dist - 1);
		}
	}
	//cout << "junction size: " << dc.size() << endl;
	return dc;
}
/*
vector<node*> node::get_all_distant_connected(unsigned long long dist){
	vector<node*> dc;
	if (dist == 1){
		dc = connections;
	}
	else if(dist != 0){
		vector<node*> dcr;
		for (unsigned long long i = 0; i < connections.size(); ++i){
			dcr = connections[i]->get_distant_connected(this, dist - 1);
			dc.insert(dc.end(),dcr.begin(),dcr.end());
		}
	}
	return dc;
}
*/
vector<node*> node::get_distant_connected(node* origin, unsigned long long dist){
	vector<node*> dc;
	if (dist == 1){
		for (unsigned long long i = 0; i < connections.size(); ++i){
			if (connections[i] != origin){
				dc.push_back(connections[i]);
			}
		}
	}
	else if(dist != 0){
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
			//else {cout << "something fucky goin on, you tryna find the streightest connection between 2 non connected nodes" << endl;}
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


double node::score_connections(double thic, double dx = 0, double dy = 0){
	double score = 0;
	for (unsigned long long i = 0; i < connections.size(); ++i){
		score += linefun_score_hessian(img, x_orig + dx, y_orig + dy, connections[i]->x, connections[i]->y, thic);
	}
	return score;
}


void node::wiggle(unsigned long long iterations, double thic){
	double best_score = score_connections(thic,x - x_orig, y - y_orig);
	double score = best_score;

	default_random_engine generator;
	//independent_bits_engine generator;
	uniform_real_distribution<double> distribution(-0.5,0.5);
	generator.seed(time(NULL));

	double dx;
	double dy;
	
	for (unsigned long long i = 0; i < iterations; ++i){
		do {
			dx =  RS * distribution(generator);
			dy =  RS * distribution(generator);
		} while ((dx*dx + dy*dy)*4 > RS*RS );

		score = score_connections(thic, dx, dy);

		if (score > best_score){
			best_score = score;
			x = x_orig + dx;
			y = y_orig + dy;
		}	
	}
}
//this function does not work and inevetably produces a stack overflow
/*
vector<node*> node::unite_junctions(unsigned long long dist){
	vector<node*> united;
	if (connections.size() <= 2){
		return united;
	}
	united.push_back(this);
	vector<node*> temp;
	vector<node*> unitable = get_all_distant_connected(dist);
	for (unsigned long long i = 0; i < unitable.size(); ++i){
		if (!node_is_in(united,unitable[i])){
			temp = unitable[i]->unite_junctions(dist);
			united.insert(united.end(),temp.begin(),temp.end());
		}
	}
	return united;
}*/

vector<node*> node::unite_junctions(unsigned long long dist){
	vector<node*> united;
	if (!is_junc(dist)){
		return united;
	}
	united.push_back(this);
	//vector<node*> temp;
	unsigned long long processed = 0;
	while (processed < united.size()){
		vector<node*> unitable = united[processed]->get_all_distant_connected(dist);
		for (unsigned long long i = 0; i < unitable.size(); ++i){
			if (unitable[i]->is_junc(dist) && !unitable[i]->is_in(united)){
				united.push_back(unitable[i]);
			}
		}
		++processed;
	}
	return united;
}

bool node::is_in(vector<node*> l){
	for (unsigned long long i = 0; i < l.size(); ++i){
		if (l[i] == this){
			return true;
		}
	}
	return false;
}

bool node::is_in(vector<united_junction*> united_junctions){
	for (unsigned long long j = 0; j < united_junctions.size(); ++j){
		vector<node*> list = united_junctions[j]->nodes;
		if (is_in(list)){
			return true;
		}
	}
	return false;
}

bool node::is_in(vector<vector<node*>> lists){
	for (unsigned long long j = 0; j < lists.size(); ++j){
		vector<node*> list = lists[j];
		for (unsigned long long i = 0; i < list.size(); ++i){
			if (list[i] == this){
				return true;
			}
		}
	}
	return false;
}


bool node::junction_in_steps(unsigned long long dist, node* direction){
	if (dist == 0){ return false; }
	else if ( direction->connections.size() > 2){ return true; }
	else if ( direction->connections.size() <= 1){ return true; } //return false to not consider scraggle as a junction here. this causes scraggle to always be considered a outgoing connection
	else{
		return direction->junction_in_steps(dist - 1,direction->connections[((find_connection_index(direction, this) + 1 )%2)]);
	}
}

bool node::junc_free_path(unsigned long long dist, node* direction, bool* found_junc){
	if ( dist == 0) {return true;}

	node* pre = this;
	node* cur = direction;

	while (cur->connections.size() == 2 && dist > 0){
		--dist;
		if (cur->connections[0] == pre){
			pre = cur;
			cur = cur->connections[1];
		}
		else if(cur->connections[1] == pre){
			pre = cur;
			cur = cur->connections[0];
		}
		else{
			cout << "Oh shit, somethings fucked!" << endl;
		}
	}
	
	if (dist == 0){ return true;}
	if (cur->j != NULL) {
		if( found_junc != NULL){
		       	//cout << "jap" << endl;
			*found_junc = true;
		}
		//cout << "jep" << endl;
		//PRINT(found_junc)
	       	return false;
	}
	//else { cout << "jup" << endl;}
	if (cur->connections.size() <= 1) {return false;}// put true here to consider scraggle shorter than dist as a valid line
	bool found_free_path = false;
	bool done = false;
	for (auto it = cur->connections.begin(); it != cur->connections.end(); ++it){
		if (*it != pre && cur->junc_free_path(dist-1, *it, &done)){
			found_free_path = true;
			if (done) {return false;}
		}
	}
	return found_free_path;
}

double node::brightness(Mat image, unsigned long long dx = 0, unsigned long long dy = 0){
/*
	PRINT(dy)
	PRINT(y)
	PRINT(x)
	PRINT((unsigned long long)y + dy)
	Size s2 = image.size();
	PRINT(s2.width)
	PRINT(s2.height)
*/
	double ret = image.at<double>((unsigned long long)y + dy,(unsigned long long)x + dx);
	//double ret = image.at<double>(883,s2.width - 1);
	//cout << "no segv" << endl;
	return ret;
}
void node::nodes_til_junc(vector<node*>& ret){
	if (connections.size() <= 2){
		ret.push_back(this);
		for (auto it = connections.begin(); it != connections.end(); ++it){
			if (!((*it)->is_in(ret))){
				(*it)->nodes_til_junc(ret);
			}
		}
	}
}

vector<node*> node::nodes_til_junc(){
	vector<node*> ret;
	ret.push_back(this);
	for (auto it = connections.begin(); it != connections.end(); ++it){
		if (!((*it)->is_in(ret))){
			(*it)->nodes_til_junc(ret);
		}
	}
	return ret;
}

unsigned long long node::del_til_junc(){
	vector<node*> to_del = nodes_til_junc();
	unsigned long long ret = to_del.size();
	//cout << "bout to delete" << endl;
	/*
	node* prev = NULL;
	for (auto it = to_del.begin(); it != to_del.end(); ++it){
		if(prev == (*it)){
			cout << "douplicate " << (*it) << " " << *(it - 1) << " " << prev << endl;
		}
		prev = (*it);
	}
	*/
	for (auto it = to_del.begin(); it != to_del.end(); ++it){
		//cout << (*it) << endl;
		delete (*it);
	}
	return ret;
}
void node::local_network(vector<node*>& ret){
	if (!is_in(ret)){
		ret.push_back(this);
		for (auto it = connections.begin(); it != connections.end(); ++it){
			(*it)->local_network(ret);
		}
	}
}

vector<node*> node::local_network(){
	vector<node*> ret;
	ret.push_back(this);
	for (auto it = connections.begin(); it != connections.end(); ++it){
		(*it)->local_network(ret);
	}
	return ret;
}

bool node::scraggle(node* prev, unsigned long long steps){
	if (steps == 0) {return false;}
	if (connections.size() <= 1) {return true;}
	if (j != NULL) {return false;}
	for (const auto& connection:connections){
		if(connection != prev && !connection->scraggle(this, steps-1)){
			return false;
		}
	}
	return true;
}

bool node::is_junc(unsigned long long dist = 0){
	unsigned long long s = connections.size();
	if (s <= 2){ return false;}
	if (dist == 0) {return true;}
	for (auto it = connections.begin(); (it != connections.end() && s > 2); ++it){
		if ((*it)->scraggle(this,dist)){
			--s;
		}
	}
	return s > 2;
}

#endif
