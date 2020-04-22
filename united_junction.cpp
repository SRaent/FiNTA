#ifndef UNITED_JUNCTION_CPP
#define UNITED_JUNCTION_CPP UNITED_JUNCTION_CPP

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>



#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "united_junction.h"
#include "analyse.cpp"

united_junction::united_junction(node* n, unsigned long long d){ 
	dist = d;
	nodes = n->unite_junctions(dist);
	find_outgoing_connections();
	determine_position();
	for (auto it = nodes.begin(); it != nodes.end(); ++it){
		(*it)->j = this;
	}
}


void united_junction::find_outgoing_connections(){
	for ( auto it = nodes.begin(); it != nodes.end(); ++it){
		for ( auto jt = (*it)->connections.begin(); jt != (*it)->connections.end(); ++jt){
			if ((*it)->junc_free_path(dist,(*jt))){
				outgoing_connections.push_back(new node*[2]);
				(outgoing_connections.back())[0] = *it;
				(outgoing_connections.back())[1] = *jt;
			}
		}
	}
}


void united_junction::determine_position(){
	x = 0;
	y = 0;
	// nodes that have multiple outgoing connections are purpously weightet stronger
	for (auto it = outgoing_connections.begin(); it != outgoing_connections.end(); ++it){
		x += (*it)[0]->x;
		y += (*it)[0]->y;
	}
	x = x/outgoing_connections.size();
	y = y/outgoing_connections.size();
}

united_junction::~united_junction(){
	for (auto it = outgoing_connections.begin(); it != outgoing_connections.end(); ++it){
		delete[] (*it);
	}
}

#endif
