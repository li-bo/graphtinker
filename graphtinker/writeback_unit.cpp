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
#include "graphtinker.h"
using namespace std;

void writeback_unit(
				moduleparams_t *moduleparams,				
				writebackunitcmd_t writebackunitcmd,
				edge_t edge,
				edge_nt * edgeblock,
				vector<edge_nt> & edge_block_array, 
				vector<edgeblock_parentinfo_t> & edgeblock_parentinfo,
				tracker_t *lvatracker,
				vertexid_t hvtx_id,
				margin_t first_wblkmargin,
				margin_t subblkmargin,
				unsigned int geni,
				unsigned int edgeupdatecmd
				#ifdef EN_CRUMPLEINONDELETE
				,unsigned int lastgenworkblockaddr
				,vector<supervertex_t> & svs
				#endif
				){	
	clusterinfo_t clusterinfo;
	unsigned int newpageindexpos=0;
	
	/** append cluster info (if necessary) */
	if(writebackunitcmd.markasclustered == YES){
		//initialize LVAentity row
		#ifdef EN_ARRAY_HEBA
		newpageindexpos = add_page(lvatracker);
		#else
		newpageindexpos = add_page2(lvatracker, edge_block_array);
		#endif
		
		//update cluster pointer
		moduleparams->clustered = YES;
		moduleparams->cptr = newpageindexpos;
		
		clusterinfo.flag = VALID;
		clusterinfo.data = newpageindexpos;
		
		edgeblock->clusterinfo = clusterinfo;
	}
	
	#ifdef EN_CRUMPLEINONDELETE
	/** once we are in REGULAR UPDATE OPERATION, and a subblock becomes clustered, the child of that subblock MUST point to an entry in the supervertex vector. Whether or not that entry is shared with another (formerly) clustered subblock depends on where the subblock is located 
	CASE 1 : if the sublock is a first child, and the subblock lies in generation 1 => then a new supervertex should be created and should be updated.
	CASE 2 : if the subblock is a first child, and the subblock DOES NOT lie in generation 1 => then a supervertex should have been created by it's founding father. thus it should simply be updated.
	CASE 3 : if the sublock is NOT a first child, and the subblock lies in generation 1 => then a new supervertex should be created and should be updated.
	CASE 4 : if the subblock is NOT a first child, and the subblock DOES NOT lie in generation 1 => then a new supervertex should be created and should be updated.
	NB: this function should be before you write the cluster info to its subblock */
	if(edgeupdatecmd != DELETEEDGE){
		if(writebackunitcmd.markasclustered == YES){
			unsigned int subblockid = subblkmargin.top/SUB_BLOCK_HEIGHT;
			unsigned int subblocksperpage = SUB_BLOCKS_PER_PAGE;
			if((subblockid == (subblocksperpage-1)) && (geni == 1)){
				
				// super vertex doesn't exist. create one
				supervertex_t sv; 
				sv.hvtx_ids.push_back(clusterinfo.data); 
				sv.geni_ofparentsubblock = geni;
				svs.push_back(sv);
				
				// update cluster info
				clusterinfo.sv_ptr = (svs.size() - 1);
				
				#ifdef EN_OTHERS
				cout<<"case 1: founding father doesn't exist. create one : "<<endl;
				cout<<"svs.size() : "<<svs.size()<<" (writeback_unit)"<<endl;
				#endif
			} else if ((subblockid == (subblocksperpage-1)) && (geni > 1)){
				
				// since it is a first child, this subblock should share the same supervertex entry as its parent subblock
				// update cluster info
				// #ifdef EN_OTHERS
				if(edge_block_array[lastgenworkblockaddr].clusterinfo.flag != VALID){ cout<<"bug1!!!!!!!!!!!!!! clusterinfo should not be empty!!! (writeback_unit)"<<endl; }
				// #endif
				
				if(edge_block_array[lastgenworkblockaddr].clusterinfo.flag != VALID){ cout<<"bug! : addr out-of-range8 (writeback_unit4) "<<endl; }
				unsigned int svs_index = edge_block_array[lastgenworkblockaddr].clusterinfo.sv_ptr;
				clusterinfo.sv_ptr = svs_index;
				
				// #ifdef EN_OTHERS
				if(svs[svs_index].hvtx_ids.empty()){ cout<<"bug should not be empty5  (writeback_unit)"<<endl; }
				// #endif
				
				// append to supervertex's entry's vector
				svs[svs_index].hvtx_ids.push_back(clusterinfo.data);
				
				#ifdef EN_OTHERS
				cout<<"case 2: founding father already exist. lastgenworkblockaddr : "<<lastgenworkblockaddr<<endl;
				cout<<"svs[svs_index].hvtx_ids.size() : "<<svs[svs_index].hvtx_ids.size()<<" (writeback_unit)"<<endl;
				cout<<"svs.size() : "<<svs.size()<<" (writeback_unit)"<<endl;
				#endif
			} else if ((subblockid != (subblocksperpage-1)) && (geni == 1)){
				
				// super vertex doesn't exist. create one
				supervertex_t sv; 
				sv.hvtx_ids.push_back(clusterinfo.data); 
				sv.geni_ofparentsubblock = geni;
				svs.push_back(sv);
				
				// update cluster info
				clusterinfo.sv_ptr = (svs.size() - 1);
				
				#ifdef EN_OTHERS
				cout<<"case 3: founding father doesn't exist. create one : "<<endl;
				cout<<"svs.size() : "<<svs.size()<<" (writeback_unit)"<<endl;
				#endif
			} else if ((subblockid != (subblocksperpage-1)) && (geni > 1)){
				
				// super vertex doesn't exist. create one
				supervertex_t sv; 
				sv.hvtx_ids.push_back(clusterinfo.data); 
				sv.geni_ofparentsubblock = geni;
				svs.push_back(sv);
				
				// update cluster info
				clusterinfo.sv_ptr = (svs.size() - 1);
				
				#ifdef EN_OTHERS
				cout<<"case 4: founding father doesn't exist. create one "<<endl;
				cout<<"svs.size() : "<<svs.size()<<" (writeback_unit)"<<endl;
				#endif
			} else { cout<<"bug! should never be seen here (WritebackUnit7) "<<endl; }
			//1001338 >= svs.size()
			if(clusterinfo.sv_ptr == 1001338){ cout<<"bug! : addr out-of-range5 (writeback_unit). clusterinfo.sv_ptr : "<<clusterinfo.sv_ptr<<", svs.size() : "<<svs.size()<<endl; }
		}
	}
	#endif
	
	/** write to DRAM */
	if(writebackunitcmd.writeback == YES){
		edgeblock->edgeinfo.flag = VALID;		
		edge_block_array[writebackunitcmd.addr] = *edgeblock;
		
		#ifdef cpuem_bugs_b1
		if(writebackunitcmd.addr>=edge_block_array.size()){ cout<<"bug! : writebackunitcmd.addr out-of-range (WritebackUnit)"<<endl; }	
		#endif
	}	
	
	/** write cluster info to all workblocks in given subblock */
	if(writebackunitcmd.markasclustered==YES){
		unsigned int subblockbaseaddr = get_edgeblock_offset(hvtx_id) + (writebackunitcmd.subblockid * WORK_BLOCKS_PER_SUBBLOCK);
		for(unsigned int id=0;id<WORK_BLOCKS_PER_SUBBLOCK;id++){
				edge_block_array[(subblockbaseaddr + id)].clusterinfo = clusterinfo;
		}
	}
	
	#ifdef EN_CRUMPLEINONDELETE
	if(writebackunitcmd.markasclustered==YES){		
		unsigned int index = clusterinfo.data - EDGEBLOCKARRAYHEIGHT;
		if(index > 261875){ cout<<"bug! out of range. writeback_unit"<<endl; }
		if(edgeblock_parentinfo[index].flag != VALID){
			edgeblock_parentinfo[index].subblockid = writebackunitcmd.subblockid;
			edgeblock_parentinfo[index].flag = VALID;
		}
	}
	#endif	
	return;
}

