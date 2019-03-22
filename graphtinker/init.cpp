#include <string.h> 
#include <iostream>
#include <vector>
#include <string>
#include "graphtinker.h" // Courtesy : Jinja 2.0
using namespace std;

unsigned int graphtinker::add_page(tracker_t * tracker){	
    //update mark 
	unsigned int pos = tracker->mark;
    tracker->mark += 1;
	return pos;
}

unsigned int graphtinker::add_page2(tracker_t * tracker, vector<edge_nt> & edge_block_array){	
	if((tracker->mark * work_blocks_per_page) >= edge_block_array.size()){ // resize only when filled		
		// resize edgeblockarray
		unsigned int newsz = edge_block_array.size() + (HEBAEXPANSIONADDITIONHEIGTH * work_blocks_per_page);	
		edge_block_array.resize(newsz);
	}	
	unsigned int pos = tracker->mark;
    tracker->mark += 1;
	return pos;
}



