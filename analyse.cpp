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
				conns[j]->get_straight_distant_connected(list[i],dist)->x;
				dx1 = conns[j]->get_straight_distant_connected(list[i],dist)->x - conns[j]->x;
				dy1 = conns[j]->get_straight_distant_connected(list[i],dist)->y - conns[j]->y;
				dx2 = conns[k]->get_straight_distant_connected(list[i],dist)->x - conns[k]->x;
				dy2 = conns[k]->get_straight_distant_connected(list[i],dist)->y - conns[k]->y;
				if( dx1 != 0 && dy1 != 0 && dx2 != 0 && dy2 != 0){
					angle = rel_half_angle(dx1,dy1,dx2,dy2);
					angles.push_back(angle);
				}
			}
		}
	}
	return angles;
}

void double_vector_to_file(string filename, vector<double> list){
	ofstream f;
	f.open(filename);
	for(unsigned long long i = 0; i < list.size(); ++i){
		f << list[i] << "\n";
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


vector<node*> find_junctions(vector<node*> list){
	vector<node*> junctions;
	for (unsigned long long i = 0; i < list.size(); ++i){
		if (list[i]->connections.size() > 2) {
			junctions.push_back(list[i]);
		}
	}
	return junctions;
}

bool node_is_in(vector<node*> list, node* element){
	for (unsigned long long i = 0; i < list.size(); ++i){
		if (list[i] == element){
			return true;
		}
	}
	return false;
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

#endif