#include <string.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ctime>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include "vertices.h" // Courtesy : Jinja 2.0
using namespace std;

vertices::vertices(){	
	cout<<"vertices::vertices : vertices constructor called"<<endl;
}
vertices::~vertices(){}

void vertices::init(unsigned int _num_vertices){
	num_vertices = _num_vertices;
	vertex_properties.resize(_num_vertices); 
	initialize();
}

vertexdata_t vertices::readdata(vertexid_t vertexid){
	return vertex_properties[vertexid].data;
}

void vertices::writedata(vertexid_t vertexid, vertexdata_t vertexdata){
	vertex_properties[vertexid].data = vertexdata;
	return;
}

vertexproperty_t vertices::readproperty(vertexid_t vertexid){
	return vertex_properties[vertexid];
}

void vertices::writeproperty(vertexid_t vertexid, vertexproperty_t vertexproperty){
	vertex_properties[vertexid] = vertexproperty;
	return;
}

void vertices::update_vertex_property(edge_t edge, unsigned int edgeupdatecmd){
	if(edgeupdatecmd == INSERTEDGE){
		vertex_properties[edge.xvtx_id].outdegree += 1;
		#ifdef UNDIRECTEDGRAPH
		vertex_properties[edge.xvtx_id].indegree += 1;
		#endif
		vertex_properties[edge.xvtx_id].flag = VALID;
	} else if (edgeupdatecmd == DELETEEDGE){		
		vertex_properties[edge.xvtx_id].outdegree -= 1;
		#ifdef UNDIRECTEDGRAPH
		vertex_properties[edge.xvtx_id].indegree -= 1;
		#endif
		vertex_properties[edge.xvtx_id].flag = VALID;
	}  else { cout<<"bug! : should never be seen here (vprop_update)"<<endl; }
	return;
}

void vertices::initialize(){
	cout<<"initializing vertices (vertices) "<<endl;
	for(unsigned int i=0; i<num_vertices; i++){
		vertex_properties[i].data = 0.15; //** change later (this is for PR) 
		vertex_properties[i].indegree = 0;
		vertex_properties[i].outdegree = 0;
		vertex_properties[i].flag = INVALID;
	}
	return;
}

void vertices::print_first_n(unsigned int n){
	for(unsigned int i=0; i<n; i++){
		cout<<"vertexid : "<<i<<" ";
		cout<<"data : "<<vertex_properties[i].data<<" ";
		cout<<"indegree : "<<vertex_properties[i].indegree<<" ";
		cout<<"outdegree : "<<vertex_properties[i].outdegree<<" ";
		cout<<"flag : "<<vertex_properties[i].flag<<" ";
		cout<<endl;
	}
	return;
}

void vertices::print_nth_vertex(unsigned int n){
	cout<<"vertexid : "<<n<<" ";
	cout<<"data : "<<vertex_properties[n].data<<" ";
	cout<<"indegree : "<<vertex_properties[n].indegree<<" ";
	cout<<"outdegree : "<<vertex_properties[n].outdegree<<" ";
	cout<<"flag : "<<vertex_properties[n].flag<<" ";
	cout<<endl;
	return;
}

vector<vertexproperty_t> & vertices::get_vertex_properties(){
	return vertex_properties;
}

vector<vertexproperty_t> & vertices::get_high_degree_vertices(unsigned int percentage){
	vertexproperty_t highest_degree = get_highest_outdegree_vertex();
	vertexproperty_t lowest_degree = get_lowest_outdegree_vertex();
	
	unsigned int count = (percentage * (highest_degree.outdegree - lowest_degree.outdegree)) / 100;
	unsigned int threshold = highest_degree.outdegree - count;
	
	for(id_t i=0; i<num_vertices; i++){
		if(vertex_properties[i].outdegree >= threshold){ 
			high_degree_vertices.push_back(vertex_properties[i]);
		}
	}
	return high_degree_vertices;
}

vector<id_t> & vertices::get_high_degree_vertices_ids(unsigned int percentage){
	vertexproperty_t highest_degree = get_highest_outdegree_vertex();
	vertexproperty_t lowest_degree = get_lowest_outdegree_vertex();
	
	unsigned int count = (percentage * (highest_degree.outdegree - lowest_degree.outdegree)) / 100;
	unsigned int threshold = highest_degree.outdegree - count;
	
	for(id_t i=0; i<num_vertices; i++){
		if(vertex_properties[i].outdegree >= threshold){ 
			high_degree_vertices_ids.push_back(i);
		}
	}
	return high_degree_vertices_ids; 
}

vertexproperty_t vertices::get_highest_outdegree_vertex(){
	vertexproperty_t maxp;
	maxp.outdegree = 0;
	for(id_t i=0; i<num_vertices; i++){
		if(vertex_properties[i].outdegree > maxp.outdegree){ maxp = vertex_properties[i]; }
	}
	return maxp;
}

vertexproperty_t vertices::get_lowest_outdegree_vertex(){
	vertexproperty_t minp;
	minp.outdegree = INFINITI;
	cout<<"vertex_properties[0].outdegree : "<<vertex_properties[0].outdegree<<endl;
	for(id_t i=0; i<num_vertices; i++){
		if(vertex_properties[i].outdegree < minp.outdegree){ minp = vertex_properties[i]; }
	}
	return minp;
}


