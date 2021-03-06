#include <string.h> 
#include <iostream>
#include <vector>
#include <string>
#include "graphtinker.h" // Courtesy : Jinja 2.0
using namespace std;

unsigned int graphtinker::add_page(tracker_t * tracker, vector<edge_nt> & edge_block_array){	
	if((tracker->mark * work_blocks_per_page) >= edge_block_array.size()){ // resize only when filled
		cout<<"add_page : resizing edge_block_array_c..."<<endl;
		edge_block_array.resize((edge_block_array.size() + (eba_c_expansion_addition_height * work_blocks_per_page)));
	}	
	unsigned int pos = tracker->mark;
    tracker->mark += 1;
	return pos;
}



