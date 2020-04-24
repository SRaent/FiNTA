#ifndef NODE_H
#define NODE_H NODE_H

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;
using namespace cv;


class united_junction;

class node {
	public:
	vector<node*> neighbors;
	vector<node*> connections;
	vector<node*>* list;
	vector<node**>* closures;
	node* mother;
	Mat* img;
	Size s;
	bool procreated;
	double x;
	double y;
	double x_orig;
	double y_orig;
	united_junction* j;
	
	
	node(double, double,vector<node*>*,Mat*);
	node(double, double,vector<node*>*,vector<node**>*,Mat*);
	node(double, double, node*);
	~node();
	void procreate(bool);
	void procreate_hessian(bool);
	void procreate_hessian_rate(bool);
	void repair_tight_closure(node*);
//	void procreate_hessian_intersect(bool);
	bool connected(node*, unsigned long long);
	bool connected(node*, node*, unsigned long long);
	vector<node*> get_distant_connected(unsigned long long);
	vector<node*> get_distant_connected(node*, unsigned long long);
	vector<node*> get_all_distant_connected(unsigned long long);
	vector<node*> get_all_distant_connected(node*, unsigned long long);
	vector<node*> get_all_distant_connected(vector<node*>&, unsigned long long);
	node* get_straight_distant_connected(node*, unsigned long long);
	double score_connections(double,double,double);
	void wiggle(unsigned long long, double);
	vector<node*> unite_junctions(unsigned long long);
	bool junction_in_steps(unsigned long long, node*);
	bool junc_free_path(unsigned long long, node*, bool*);


	bool is_in(vector<node*>);
	bool is_in(vector<vector<node*>>);
	bool is_in(vector<united_junction*>);

	double brightness(Mat, unsigned long long, unsigned long long);

	unsigned long long del_til_junc();
	vector<node*> nodes_til_junc();
	void nodes_til_junc(vector<node*>&);

	void local_network(vector<node*>&);
	vector<node*> local_network();

	bool is_junc(unsigned long long);
	bool scraggle(node*, unsigned long long);
};
	

#endif
