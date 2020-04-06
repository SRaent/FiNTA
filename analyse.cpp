#ifndef ANALYSE_CPP
#define ANALYSE_CPP ANALYSE_CPP

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "mathfun.cpp"
#include "node.h"
#include "node.cpp"
#include "united_junction.h"
#include "united_junction.cpp"


#define PI (double)3.1415926535897932384626433832795

unsigned long long find_node_list_position(node* n, vector<node*> list){
	for (unsigned long long i = 0; i < list.size(); ++i){
		if (list[i] == n){
			return i;
		}
	}
	return 0;
}

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

node* find_next_loop_node(node* prev, node* n, long long sign){
	double prevang = atan2(n->x - prev->x, n->y - prev->y);
	double minang = numeric_limits<double>::infinity();
	double ang = 0;
	unsigned long long index = 0;
	
	for (unsigned long long i = 0; i < n->connections.size(); ++i){
		ang = ((double)sign) * (atan2(n->x - n->connections[i]->x, n->y - n->connections[i]->y) - prevang);
		while (ang < 0){
			ang += 2.0 * PI;
		}
		while (ang >= 2.0 * PI){
			ang -= 2.0 * PI;
		}
		if (ang < minang && ang > 0.0){
			minang = ang;
			index = i;
		}
	}
	return n->connections[index];
}


bool node_is_in(vector<node*> list, node* element){
	return element->is_in(list);
}

bool node_is_in(vector<vector<node*>> lists, node* element){
	return element->is_in(lists);
}


bool node_list_eql(vector<node*> loop1, vector<node*> loop2){
	bool eql = true;
	for(auto it = loop1.begin(); it != loop1.end() && eql; ++it){
		eql = node_is_in(loop2,*it);
	}
	for(auto it = loop2.begin(); it != loop2.end() && eql; ++it){
		eql = node_is_in(loop2,*it);
	}
	return eql;
}

//end is a misleading variable name
vector<node*> find_loop(node* prev, node* n, node* start, node* end, long long sign){
	vector<node*> ret;
	node* next_loop_node;
//	cout << n->x << " " << n->y << " " << n << " " << prev << " " << start << " ";
	if (n == start && find_next_loop_node(prev, n, sign) == end){
		ret.push_back(n);
		return ret;
	}
	else{
		next_loop_node = find_next_loop_node(prev, n, sign);
		
//		cout << find_connection_index(n, prev) << " " << index << " " << n->connections.size() << endl;
		ret = find_loop(n, next_loop_node, start, end, sign);
		ret.push_back(n);
	}
	return ret;
}

vector<node*> find_loop(node** closure, long long sign){
	return find_loop(closure[0],closure[1],closure[0],closure[1],sign);
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
	
	for (unsigned long long i = 0; i < ret.size() - 1; ++i){
		for (unsigned long long j = i + 1; j < ret.size(); ++j){
			if(node_list_eql(ret[i],ret[j])){
				ret.erase(ret.begin() + j);
				--j;
			}
		}
	}
	
	return ret;
}



void only_loops(vector<node*> &list, vector<node**> &closures){
	//PRINT(closures.size())
	bool buisy = true;
	while (buisy){
		
		buisy = false;
		for (unsigned long long i = 0; i < list.size(); ++i){
			if (list[i]->connections.size() <= 1){
				//cout << "about to delete node" << endl;
				delete (list[i]);
				--i;
				buisy = true;
			}
		}
	}
	/*
	// this is for testing the delete function
	for (unsigned long long i = 0; i < list.size(); ++i){
		if (list[i]->connections.size() <= 1){
			cout << "scraggle not deleted!!!!" << endl;
		}
		long test = 0;
		for (unsigned long long j = 0; j < list[i]->connections.size(); ++j){
			for (unsigned long long k = 0; k < list[i]->connections[j]->connections.size(); ++k){
				if (list[i]->connections[j]->connections[k] == list[i]){
					++test;
				}
			}
		}
		//cout << "c" << flush;
		if (test != list[i]->connections.size()) {
			cout << "fishy!!!" << endl;
			PRINT(list[i]->connections.size())
		}
	}
	*/
	//PRINT(closures.size())
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
	return abs(area * scaling_factor * scaling_factor/2.0);
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
	return length * scaling_factor;
}


double inside_loop_area(vector<node*> loop, double thickness){
	return loop_area(loop) - (loop_length(loop)*thickness * scaling_factor/2.0) + PI * thickness * thickness * 0.25 * scaling_factor * scaling_factor; // the last term is a approximation for the overlapping of the assumed fiber thickness
}


vector<double> con_angles(vector<node*> list){
	vector<double> angles;
	double angle;
	double dx1;
	double dy1;
	double dx2;
	double dy2;
	for(unsigned long long i = 0; i < list.size(); ++i){
		vector<node*> conns = list[i]->connections;
		double x = list[i]->x;
		double y = list[i]->y;
		for(unsigned long long j = 0; j < conns.size(); ++j){
			for (unsigned long long k = j + 1; k < conns.size(); ++k){
				dx1 = conns[j]->x - x;
				dy1 = conns[j]->y - y;
				dx2 = conns[k]->x - x;
				dy2 = conns[k]->y - y;
				angle = rel_half_angle(dx1,dy1,dx2,dy2);
				angles.push_back(angle);
			}
		}
	}
	return angles;
}

vector<double> con_angles_long(vector<node*> list, unsigned long long dist = 1){
	vector<double> angles;
	double dx1;
	double dy1;
	double dx2;
	double dy2;
	for(unsigned long long i = 0; i < list.size(); ++i){
		vector<node*> conns = list[i]->connections;
		for(unsigned long long j = 0; j < conns.size(); ++j){
			for (unsigned long long k = j + 1; k < conns.size(); ++k){
				dx1 = conns[j]->get_straight_distant_connected(list[i],dist)->x - conns[j]->x;
				dy1 = conns[j]->get_straight_distant_connected(list[i],dist)->y - conns[j]->y;
				dx2 = conns[k]->get_straight_distant_connected(list[i],dist)->x - conns[k]->x;
				dy2 = conns[k]->get_straight_distant_connected(list[i],dist)->y - conns[k]->y;
				if( (dx1 != 0 || dy1 != 0) && (dx2 != 0 || dy2 != 0)){
					angles.push_back(rel_half_angle(dx1,dy1,dx2,dy2));
				}
			}
		}
	}
	return angles;
}

vector<double> con_angles_long_curvature(vector<node*> list, unsigned long long dist = 1){
	vector<double> angles;
	double dx1;
	double dy1;
	double dx2;
	double dy2;
	double a;
	double b;
	double c;
	double x;
	double y;
	Vec3d vals;
	for(unsigned long long i = 0; i < list.size(); ++i){
		vector<node*> conns = list[i]->connections;
		for(unsigned long long j = 0; j < conns.size(); ++j){
			for (unsigned long long k = j + 1; k < conns.size(); ++k){
				x = conns[j]->get_straight_distant_connected(list[i],dist)->x;
				y = conns[j]->get_straight_distant_connected(list[i],dist)->y;
				vals = list[i]->img->at<Vec3d>((unsigned long long)y,(unsigned long long)x);
				dx1 = vals[0] - vals[2] - sqrt(pow(vals[0]-vals[2],2) + pow(2.0*vals[1],2));
				dy1 = 2.0 * vals[1];
				x = conns[k]->get_straight_distant_connected(list[i],dist)->x;
				y = conns[k]->get_straight_distant_connected(list[i],dist)->y;
				vals = list[i]->img->at<Vec3d>((unsigned long long)y,(unsigned long long)x);
				dx2 = vals[0] - vals[2] - sqrt(pow(vals[0]-vals[2],2) + pow(2.0*vals[1],2));
				dy2 = 2.0 * vals[1];
				if( (dx1 != 0 || dy1 != 0) && (dx2 != 0 || dy2 != 0)){
					angles.push_back(rel_quater_angle(dx1,dy1,dx2,dy2));
				}
				//never observed
				//else{
					//cout << "einer der vektoren = (0,0) " << endl;
				//}
			}
		}
	}
	return angles;
}


vector<double> all_con_angles_long_curvature(vector<node*> list, unsigned long long dist = 1){
	vector<double> angles;
	double dx1;
	double dy1;
	double dx2;
	double dy2;
	double a;
	double b;
	double c;
	double x;
	double y;
	Vec3d vals;
	for(unsigned long long i = 0; i < list.size(); ++i){
		vector<node*> conns = list[i]->get_distant_connected(dist);
		for(unsigned long long j = 0; j < conns.size(); ++j){
			for (unsigned long long k = j + 1; k < conns.size(); ++k){
				x = conns[j]->x;
				y = conns[j]->y;
				vals = list[i]->img->at<Vec3d>((unsigned long long)y,(unsigned long long)x);
				dx1 = vals[0] - vals[2] - sqrt(pow(vals[0]-vals[2],2) + pow(2.0*vals[1],2));
				dy1 = 2.0 * vals[1];
				x = conns[k]->x;
				y = conns[k]->y;
				vals = list[i]->img->at<Vec3d>((unsigned long long)y,(unsigned long long)x);
				dx2 = vals[0] - vals[2] - sqrt(pow(vals[0]-vals[2],2) + pow(2.0*vals[1],2));
				dy2 = 2.0 * vals[1];
				if( (dx1 != 0 || dy1 != 0) && (dx2 != 0 || dy2 != 0)){
					angles.push_back(rel_quater_angle(dx1,dy1,dx2,dy2));
				}
				//else{
				//	cout << "einer der vektoren = (0,0) " << endl;
				//}
			}
		}
	}
	return angles;
}

void double_vector_to_file(string filename, vector<double> list){
	ofstream f;
	f.open(filename);
	if (f.is_open()){
		for(unsigned long long i = 0; i < list.size(); ++i){
			f << list[i] << "\n";
		}
	}
	else {
		cout << "WARNING: File: \"" << filename << "\" could not be opened" << endl;
	}
	f.close();
}
void ull_vector_to_file(string filename, vector<unsigned long long> list){
	ofstream f;
	f.open(filename);
	if (f.is_open()){
		for(unsigned long long i = 0; i < list.size(); ++i){
			f << list[i] << "\n";
		}
	}
	else {
		cout << "WARNING: File: \"" << filename << "\" could not be opened" << endl;
	}
	f.close();
}

double max_loop_area(vector<vector<node*>> loops){
	double max_a = 0;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		max_a = max(max_a,loop_area(loops[l]));
	}
	return max_a;
}

double total_loop_area(vector<vector<node*>> loops){
	double tot_a = 0;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		tot_a += loop_area(loops[l]);
	}
	return tot_a/2.0;
}

vector<double> find_loop_areas(vector<vector<node*>> loops){
	vector<double> areas;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		areas.push_back(loop_area(loops[l]));
	}
	return areas;
}

vector<double> find_loop_lengths(vector<vector<node*>> loops){
	vector<double> lengths;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		lengths.push_back(loop_length(loops[l]));
	}
	return lengths;
}

vector<double> find_loop_lengths_wo_max(vector<vector<node*>> loops){
	vector<double> lengths;
	double length = 0;
	double max_length = 0;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		length = loop_length(loops[l]);
		if (length > max_length){
			if (max_length != 0){
				lengths.push_back(max_length);
			}
			max_length = length;
		}
		else{
			lengths.push_back(length);
		}
	}
	return lengths;
}


// the loop with the biggest area encompasses all other loops because of the way the loops are found. not considering the biggest area is not manipulating data
vector<double> find_loop_areas_wo_max(vector<vector<node*>> loops){
	vector<double> areas;
	double area = 0;
	double max_area = 0;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		area = loop_area(loops[l]);
		if (area > max_area){
			if (max_area != 0){
				areas.push_back(max_area);
			}
			max_area = area;
		}
		else{
			areas.push_back(area);
		}
	}
	return areas;
}

vector<double> find_loop_areas_wo_max_w_diam(vector<vector<node*>> loops,double thickness){
	vector<double> areas;
	double area = 0;
	double max_area = 0;
	for (unsigned long long l = 0; l < loops.size(); ++l){
		area = inside_loop_area(loops[l],thickness);
		if (area > max_area){
			if (max_area != 0){
				areas.push_back(max_area);
			}
			max_area = area;
		}
		else if (area > 0.0){
			areas.push_back(area);
		}
//		else{
//		 areas.push_back(0.0);
//		}
	}
	return areas;
}





vector<node*> find_junctions(vector<node*> list){
	vector<node*> junctions;
	for (unsigned long long i = 0; i < list.size(); ++i){
		if (list[i]->connections.size() > 2) {
			junctions.push_back(list[i]);
		}
	}
	return junctions;
}

void del_ident_node_lsts(vector<vector<node*>>& jc){
	//cout << " a" << flush;
	//PRINT(jc.size())
	for (unsigned long long i = 0; i < jc.size() - 1 && jc.size() > 0; ++i){
		//cout << " b" << flush;
		for (unsigned long long j = i + 1; j < jc.size(); ++j){
			//cout << " c" << flush;
			if(node_list_eql(jc[i],jc[j])){
				jc.erase(jc.begin() + j);
				--j;
			}
		}
	}
}


vector<node*> junc_line(node*, node*);
double line_length(vector<node*>);
vector<double> junction_distances(vector<node*> junc){
	vector<double> jd;
	vector<vector<node*>> jc;
	
	
	for(auto it = junc.begin(); it != junc.end(); ++it){
		for(auto jt = (*it)->connections.begin(); jt != (*it)->connections.end(); ++jt){
			jc.push_back(junc_line(*it,*jt));
		}
	}
	
	del_ident_node_lsts(jc);
	
	for (unsigned long long i = 0; i < jc.size(); ++i){
		jd.push_back(line_length(jc[i]) * scaling_factor);
	}
	
	
	return jd;
}

vector<united_junction*> find_united_junctions(vector<node*> list, unsigned long long dist){
	vector<united_junction*> united_junctions;
	for (unsigned long long i = 0; i < list.size(); ++i){
		if (list[i]->connections.size() > 2 && !list[i]->is_in(united_junctions)){
			united_junctions.push_back(new united_junction(list[i],dist));
		}
	}
	return united_junctions;
}

vector<double> united_junction_distances(vector<united_junction*> u_juncs){
	vector<double> jd;
	vector<vector<node*>> jc;

	//PRINT((*(u_juncs.begin()))->outgoing_connections.size())
	//PRINT(u_juncs.size())

	for (auto it = u_juncs.begin(); it != u_juncs.end(); ++it){
		for (auto jt = (*it)->outgoing_connections.begin(); jt != (*it)->outgoing_connections.end(); ++jt){
			jc.push_back(junc_line((*jt)[0],(*jt)[1]));
		}
	}
	//cout << "find united junction distances" << endl;

	del_ident_node_lsts(jc);
	//cout << "done deleting equal nodelists" << endl;
	
	double dx_start = 0;
	double dy_start = 0;
	double dx_end = 0;
	double dy_end = 0;
	double d_start = 0;
	double d_end = 0;
	for (unsigned long long i = 0; i < jc.size(); ++i){
		//cout << " c" << endl;
		//PRINT(line_length(jc[i]))
		//PRINT(jc[i].back()->connections.size())
		//PRINT(jc[i].back()->y)
		//PRINT(jc[i].back()->j->y)
		//PRINT(jc[i].back()->x)
		//PRINT(jc[i].back()->j->x)
		//PRINT(jc[i][0]->connections.size())
		//PRINT(jc[i][0]->y)
		//PRINT(jc[i][0]->j->y)
		//PRINT(jc[i][0]->x)
		//PRINT(jc[i][0]->j->x)
		dx_start = jc[i][0]->x - jc[i][0]->j->x;
		dy_start = jc[i][0]->y - jc[i][0]->j->y;
		d_start = sqrt(sqr(dx_start) + sqr(dy_start));
		if (jc[i].back()->connections.size() <= 1){
			d_end = 0;
		}
		else {
			dx_end = jc[i].back()->x - jc[i].back()->j->x;
			dy_end = jc[i].back()->y - jc[i].back()->j->y;
			d_end = sqrt(sqr(dx_end) + sqr(dy_end));
		}
		jd.push_back((line_length(jc[i]) + d_start + d_end) * scaling_factor);
		//jd.push_back((line_length(jc[i]) + sqrt(sqr((*(jc[i].back()))->y - (*(jc[i].back()))->j->y) + sqr((*(jc[i].back()))->x - (*(jc[i].back()))->j->x)) + sqrt(sqr((*(jc[i].begin()))->y - (*(jc[i].begin()))->j->y) + sqr((*(jc[i].begin()))->x - (*(jc[i].begin()))->j->x))) * scaling_factor);
	}
	return jd;
}
	

vector<node*> find_line_naive(node* n1, node* n2, double curve_angle, vector<node*> &work_list, vector<node*> junctions, unsigned long long unnessesary = 0){
	vector<node*> line;
	line.push_back(n2);
	work_list.push_back(n2);
	vector<node*> conns = n2->connections;
	double dx1 = 0;
	double dy1 = 0;
	double dx2 = 0;
	double dy2 = 0;
	double angle = 0;
	double max_angle = 0;
	double min_angle = numeric_limits<double>::infinity();
	unsigned long long max_angle_index = 0;
	unsigned long long min_angle_index = 0;
	bool buisy = true;
	
	while (buisy){
		buisy = false;
		conns = n2->connections;
		dx1 = n1->x - n2->x;
		dy1 = n1->y - n2->y;
		max_angle = 0;
		min_angle = numeric_limits<double>::infinity();
		for(unsigned long long j = 0; j < conns.size(); ++j){
			if (conns[j] != n1){
				dx2 = conns[j]->x - n2->x;
				dy2 = conns[j]->y - n2->y;
				angle = rel_half_angle(dx1,dy1,dx2,dy2);
				
				if (angle > max_angle){
					max_angle = angle;
					max_angle_index = j;
				}
				if (angle < min_angle){
					min_angle = angle;
					min_angle_index = j;
				}
				//PRINT(angle/PI)
			}
		}
		if (PI - max_angle < curve_angle && (!(node_is_in(work_list,conns[max_angle_index])) || node_is_in(junctions,conns[max_angle_index]))) {
			n1 = n2;
			n2 = conns[max_angle_index];
			line.push_back(n2);
			work_list.push_back(n2);
			//cout << "line length raised " << max_angle_index << endl;
			buisy = true;
		}
		//else {cout << "line done" << endl;}
	}
	return line;
}

vector<node*> find_whole_line(node* n, double curve_angle, vector<node*> &work_list, vector<node*> junctions, unsigned long long unnessesary = 0){
	vector<node*> line;
	if (n->connections.size() == 2 && (node_is_in(junctions,n->connections[0]) || !(node_is_in(work_list,n->connections[0]))) && (node_is_in(junctions,n->connections[1]) || !(node_is_in(work_list,n->connections[1])))){
		vector<node*> lp1 = find_line_naive(n,n->connections[0], curve_angle, work_list, junctions, unnessesary);
		vector<node*> lp2 = find_line_naive(n,n->connections[1], curve_angle, work_list, junctions, unnessesary);
		for (long long i = lp1.size() - 1; i >= 0; --i){
			line.push_back(lp1[i]);
		}
		line.push_back(n);
		line.insert(line.end(),lp2.begin(),lp2.end());
	}
	else if (n->connections.size() == 1 && (node_is_in(junctions,n->connections[0]) || !(node_is_in(work_list,n->connections[0])))){
		line = find_line_naive(n,n->connections[0], curve_angle, work_list, junctions, unnessesary);
	}
	work_list.push_back(n);
	return line;
}

vector<vector<node*>> find_lines(vector<node*> list, double angle, unsigned long long rec_steps = 0){
	vector<node*> junctions = find_junctions(list);
	vector<vector<node*>> lines;
	vector<node*> work_list;
	vector<node*> line;
	for (unsigned long long i = 0; i < list.size(); ++i){
		if((list[i]->connections.size() == 1 || list[i]->connections.size() == 2) && !(node_is_in(work_list, list[i]))){
			line = find_whole_line(list[i], angle, work_list, junctions);
			if (line.size() > 2){
				lines.push_back(line);
			}
		}
	}
	return lines;
}


vector<node*> junc_line(node* junc, node* dir){
	vector<node*> line;
	
	line.push_back(junc);
	line.push_back(dir);
	
	node* pre = junc;
	node* cur = dir;
	
	while (cur->connections.size() == 2){
		if (cur->connections[0] == pre){
			pre = cur;
			cur = cur->connections[1];
			line.push_back(cur);
		}
		else if(cur->connections[1] == pre){
			pre = cur;
			cur = cur->connections[0];
			line.push_back(cur);
		}
		else{
			cout << "Oh shit, somethings fucked!" << endl;
			line[-1];
		}
	}
	
	return line;
}

double line_length(vector<node*> line){
	double l = 0;
	for(unsigned long long i = 0; i < line.size() - 1; ++i){
		l += sqrt(sqr(line[i]->x - line[i+1]->x) + sqr(line[i]->y - line[i+1]->y));
	}
	return l;
}
double network_length(vector<node*> list){
	double len = 0;
	for (auto it = list.begin(); it != list.end(); ++it){
		for (auto it2 = (*it)->connections.begin(); it2 != (*it)->connections.end(); ++it2){
			len += sqrt(pow((*it)->x - (*it2)->x,2) + pow((*it)->y - (*it2)->y,2));
		}
	}
	return len/2;
}

#endif
