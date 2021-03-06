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
#include "graphtinker.h"
using namespace std;

#ifdef EN_LLGDS	
void graphtinker::ll_insert(
			edge_t edge,
			moduleparams_t * moduleparams,
			insertparams_t * insertparams,
			vector<ll_edgeblock_t> & ll_edge_block_array,
			vector<ll_logicalvertexentity_t> & ll_lva,
			ll_eba_tracker_t * ll_eba_tracker,
			unsigned int geni
			){
	unsigned int tailid=0;
	unsigned int offsetaddr=0;
	unsigned int localoffsetaddr=0;
	unsigned int lvaindex=0;
	unsigned int ONE=1;
	
	/// calculate lvaindex
	lvaindex = edge.xvtx_id / LVACOARSENESSWIDTH;
	
	/// resize ll_lva if full 
	if(lvaindex >= ll_lva.size()){
		ll_lva.resize((ll_lva.size() + ll_lva_expansion_addition_height));
		cout<<"ll_insert : resizing ll_lva... "<<endl;
	}
	#ifdef EN_BUGCHECK
	if(lvaindex>=ll_lva.size()){cout<<"graphtinker::ll_insert : out-of-range 1 lvaindex = "<<lvaindex<<", ll_lva.size() = "<<ll_lva.size()<<endl;}
	#endif
	
	/// retrieve ll_lva entity
	ll_logicalvertexentity_t entity = ll_lva[lvaindex];	
	
	/// resize ll_edge_block_array if full	
	if(((entity.lastlocalbaseaddr) >= ll_edge_block_array.size()) || (ll_eba_tracker->ptraddr >= ll_edge_block_array.size())){
		cout<<"ll_insert : resizing ll_edge_block_array..."<<endl;
		ll_edge_block_array.resize((ll_edge_block_array.size() + ll_eba_expansion_addition_height));
	}
	
	/// check if we need to initialize first
	if(entity.flag!=VALID){
		//never used before, initialize first before use
		// create
		entity.baseaddr = ll_eba_tracker->ptraddr;
		entity.lastlocalbaseaddr = ll_eba_tracker->ptraddr;
		entity.lastlocaladdr = 0;
		entity.totaledgecount=0;
		entity.flag = VALID;
		
		// insert
		unsigned int currentlocalbaseaddr = ll_eba_tracker->ptraddr;
		ll_edge_block_array[currentlocalbaseaddr].ll_edgeblock[0] = edge;
		ll_edge_block_array[currentlocalbaseaddr].metadata.edgecount = 1;
		ll_edge_block_array[currentlocalbaseaddr].metadata.nextcptr = NAv; // next edgeblock location
		ll_edge_block_array[currentlocalbaseaddr].metadata.currcptr = ll_eba_tracker->ptraddr; // this current edgeblock location
		ll_edge_block_array[currentlocalbaseaddr].metadata.prevcptr = NAv;
		
		#ifdef EN_BUGCHECK
		if((ll_eba_tracker->ptraddr)>=ll_edge_block_array.size()){cout<<"bug! : out-of-range 31 (GraphTinkerLL)"<<endl;}
		#endif
		
		// increment tracker
		ll_eba_tracker->ptraddr += 1; // update
	}
	
	/// insert edge
	else if (entity.flag==VALID) {
		if((entity.totaledgecount % LLEDGEBLOCKSIZE) == 0){ 			
			/// full, allocate new edgeblock and insert in it
			
			/// update
			unsigned int currentlocalbaseaddr = entity.lastlocalbaseaddr;
			if(currentlocalbaseaddr >= ll_edge_block_array.size()){ cout<<"graphtinker::ll_insert : out-of-range 22, entity.lastlocalbaseaddr = "<<entity.lastlocalbaseaddr<<", ll_edge_block_array.size() = "<<ll_edge_block_array.size()<<endl; }		
			ll_edge_block_array[currentlocalbaseaddr].metadata.nextcptr = ll_eba_tracker->ptraddr; // next edgeblock location
			
			/// update
			entity.lastlocalbaseaddr = ll_eba_tracker->ptraddr;
			entity.lastlocaladdr = 0;
			
			/// insert
			unsigned int nextlocalbaseaddr = ll_eba_tracker->ptraddr;
			#ifdef EN_BUGCHECK
			if((ll_eba_tracker->ptraddr)>=ll_edge_block_array.size()){cout<<"bug! : out-of-range 3 (GraphTinkerLL)"<<endl;}
			#endif
			ll_edge_block_array[nextlocalbaseaddr].ll_edgeblock[entity.lastlocaladdr] = edge;
			ll_edge_block_array[nextlocalbaseaddr].metadata.edgecount = 1;
			ll_edge_block_array[nextlocalbaseaddr].metadata.nextcptr = NAv; // unknown next edgeblock location
			ll_edge_block_array[nextlocalbaseaddr].metadata.currcptr = ll_eba_tracker->ptraddr; // current edgeblock location
			ll_edge_block_array[nextlocalbaseaddr].metadata.prevcptr = ll_edge_block_array[currentlocalbaseaddr].metadata.currcptr;
			
			/// increment
			ll_eba_tracker->ptraddr += 1; // update
			
		} else { 
			/// not full, insert edge			
			ll_edge_block_array[entity.lastlocalbaseaddr].ll_edgeblock[entity.lastlocaladdr] = edge;
			ll_edge_block_array[entity.lastlocalbaseaddr].metadata.edgecount += 1;					
		}
	}
	
	/// append entity.lastlocalbaseaddr & entity.lastlocaladdr to insertparams 
	/// this is how every edge in edge_block_array gets its pointer to ll_edge_block_array
	#ifdef EN_LLGDS
	moduleparams->ll_localbaseaddrptr_x = entity.lastlocalbaseaddr;
	moduleparams->ll_localaddrptr_x = entity.lastlocaladdr;
	#endif
	
	/// common updates 
	entity.totaledgecount += 1; 
	entity.lastlocaladdr += 1; 
	
	/// submit changes
	#ifdef EN_BUGCHECK
	if(lvaindex>=ll_lva.size()){cout<<"bug! : out-of-range 4 (GraphTinkerLL). lvaindex = "<<lvaindex<<", ll_lva.size() = "<<ll_lva.size()<<endl;}
	#endif	
	ll_lva[lvaindex] = entity;
	return; 
}

void graphtinker::ll_update(
			edge_t edge,
			vertexid_t ll_localbaseaddrptr,
			vertexid_t ll_localaddrptr,
			vector<ll_edgeblock_t> & ll_edge_block_array
			){
	ll_edge_block_array[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].edgew += 1;
	return; 
}

void graphtinker::ll_delete(
			edge_t edge,
			vertexid_t ll_localbaseaddrptr,
			vertexid_t ll_localaddrptr,
			vector<ll_edgeblock_t> & ll_edge_block_array
			){
	ll_edge_block_array[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag = INVALID;
	return; 
}

void graphtinker::ll_deleteandcrumplein(
			edge_t edge,
			vertexid_t ll_localbaseaddrptr,
			vertexid_t ll_localaddrptr,
			vector<ll_edgeblock_t> & ll_edge_block_array,
			vector<ll_logicalvertexentity_t> & ll_lva,
			ll_eba_tracker_t * ll_eba_tracker,
			vector<edge_nt> & edge_block_array_m,
			vector<edge_nt> & edge_block_array_c,
			unsigned int geni){
	// find partiton
	unsigned int work_blocks_per_subblock = work_blocks_per_subblock;	
	edge_t nedge;
	
	// delete edge
	ll_edge_block_array[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag = INVALID;
	// return;
	
	// pop-out from end and pop-in to hole where edge was deleted			
	unsigned int lvaindex = edge.xvtx_id / LVACOARSENESSWIDTH;
	if(lvaindex >= ll_lva.size()){ cout<<"bug! : addr out-of-range11 (gds_ll) "<<endl; }
	ll_logicalvertexentity_t entity = ll_lva[lvaindex];	
	if(entity.flag != VALID){ cout<<"bug! : something wrong4 (gds_ll) "<<endl; }
	if(entity.lastlocalbaseaddr == NAv){ return; } //****
	
	unsigned int localbaseaddr = entity.lastlocalbaseaddr;
	unsigned int localaddr = entity.lastlocaladdr;
	// return;
	
	if(localaddr > 0){
		// not empty, fetch from edgeblock
		
		unsigned int localaddrofvalidedge = localaddr - 1;
		
		if((localbaseaddr == ll_localbaseaddrptr) && (localaddrofvalidedge == ll_localaddrptr)){
			// the last was the hole

			// update entity <only these fields>
			entity.lastlocaladdr = entity.lastlocaladdr - 1;
			entity.totaledgecount = entity.totaledgecount - 1;
		
			// update
			ll_lva[lvaindex] = entity;
			
			// get out
			return; 
		} else {

			// update entity <only these fields>
			entity.lastlocaladdr = entity.lastlocaladdr - 1;
			entity.totaledgecount = entity.totaledgecount - 1;
			
			// pop-out, pop-in
			if(localbaseaddr >= ll_edge_block_array.size()){ cout<<"bug! : addr out-of-range7 (gds_ll) "<<endl; }
			if(localaddrofvalidedge >= LLEDGEBLOCKSIZE){ cout<<"bug! : addr out-of-range88. entity.lastlocaladdr : "<<entity.lastlocaladdr<<", LLEDGEBLOCKSIZE : "<<LLEDGEBLOCKSIZE<<" (gds_ll) "<<endl; }
			if(ll_localbaseaddrptr >= ll_edge_block_array.size()){ cout<<"bug! : addr out-of-range9 (gds_ll) "<<endl; }
			if(ll_localaddrptr >= LLEDGEBLOCKSIZE){ cout<<"bug! : addr out-of-range10. ll_localaddrptr : "<<ll_localaddrptr<<", LLEDGEBLOCKSIZE : "<<LLEDGEBLOCKSIZE<<" (gds_ll) "<<endl; }
			
			// pop-out
			nedge = ll_edge_block_array[localbaseaddr].ll_edgeblock[localaddrofvalidedge];
			
			// invalidate //***
			if(nedge.flag != VALID){ cout<<"bug! : something wrong551. localbaseaddr : "<<localbaseaddr<<", localaddrofvalidedge : "<<localaddrofvalidedge<<", ll_localbaseaddrptr : "<<ll_localbaseaddrptr<<", ll_localaddrptr : "<<ll_localaddrptr<<" (gds_ll) "<<endl; }
			
			ll_edge_block_array[localbaseaddr].ll_edgeblock[localaddrofvalidedge].flag = INVALID;
			
			// pop in
			if(ll_edge_block_array[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag == VALID){ cout<<"bug! : something wrong457. ll_localbaseaddrptr : "<<ll_localbaseaddrptr<<", ll_localaddrptr : "<<ll_localaddrptr<<" (gds_ll) "<<endl; }
			ll_edge_block_array[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr] = nedge;
			
			// redirect pointer (heba -> ll)
			unsigned int workblockaddr = get_edgeblock_offset(nedge.heba_hvtx_id) + nedge.heba_workblockid;
			if(nedge.which_gen_is_the_main_copy_located == 1){
				if(workblockaddr >= edge_block_array_m.size()){
					cout<<"graphtinker::ll_functions : addr out-of-range6 : nedge.which_gen_is_the_main_copy_located : "<<nedge.which_gen_is_the_main_copy_located<<", workblockaddr : "<<workblockaddr<<endl; 
				}
				edge_block_array_m[workblockaddr].edges[nedge.heba_loffset].ll_localbaseaddrptr = ll_localbaseaddrptr;
				edge_block_array_m[workblockaddr].edges[nedge.heba_loffset].ll_localaddrptr = ll_localaddrptr;
			} else {
				if(workblockaddr >= edge_block_array_c.size()){ 
					cout<<"graphtinker::ll_functions : addr out-of-range7 : nedge.which_gen_is_the_main_copy_located : "<<nedge.which_gen_is_the_main_copy_located<<", workblockaddr : "<<workblockaddr<<", nedge.heba_hvtx_id : "<<nedge.heba_hvtx_id<<", edge_block_array_c.size() : "<<edge_block_array_c.size()<<endl; 
				}
				edge_block_array_c[workblockaddr].edges[nedge.heba_loffset].ll_localbaseaddrptr = ll_localbaseaddrptr;
				edge_block_array_c[workblockaddr].edges[nedge.heba_loffset].ll_localaddrptr = ll_localaddrptr;
			}

			// update
			ll_lva[lvaindex] = entity;
			return;
		}
	
	} else if(localaddr == 0){
		// empty, free this edgeblock and fetch from its predecessor new edgeblock
		
		unsigned int localaddrofvalidedge = (LLEDGEBLOCKSIZE - 1);
		
		if(entity.lastlocalbaseaddr >= ll_edge_block_array.size()){ cout<<"bug! : addr out-of-range67 (gds_ll) "<<endl; }
		unsigned int prevcptr = ll_edge_block_array[entity.lastlocalbaseaddr].metadata.prevcptr; // next edgeblock location
		if(prevcptr != NAv){ if(prevcptr >= ll_edge_block_array.size()){ cout<<"bug! : addr out-of-range69. prevcptr : "<<prevcptr<<", ll_edge_block_array.size() : "<<ll_edge_block_array.size()<<" (gds_ll) "<<endl; }}
		unsigned int prevlocalbaseaddr;
		if(prevcptr != NAv){ prevlocalbaseaddr = ll_edge_block_array[prevcptr].metadata.currcptr; } //****
		if(prevcptr != NAv){ if(prevlocalbaseaddr != prevcptr){ cout<<"bug! : incorrect value. prevcptr : "<<prevcptr<<", prevlocalbaseaddr : "<<prevlocalbaseaddr<<" (gds_ll) "<<endl; }}
		
		localbaseaddr = prevcptr;
		
		if(((localbaseaddr == ll_localbaseaddrptr) && (localaddrofvalidedge == ll_localaddrptr)) || (prevcptr == NAv)){
			// the last was the hole
			
			// update entity 
			entity.lastlocalbaseaddr = prevcptr; //if(prevcptr != NAv){ entity.lastlocalbaseaddr = prevcptr; } else { entity.lastlocalbaseaddr = NAv; } //****
			entity.lastlocaladdr = (LLEDGEBLOCKSIZE - 1);
			entity.totaledgecount = entity.totaledgecount - 1; //***![check later]
			
			if(prevcptr != NAv){ ll_edge_block_array[prevcptr].metadata.nextcptr = NAv; } //****
		
			// update
			ll_lva[lvaindex] = entity;
			
			// get out
			return; 
		} else {
		
			// update entity 
			entity.lastlocalbaseaddr = prevcptr; //****
			entity.lastlocaladdr = (LLEDGEBLOCKSIZE - 1);
			entity.totaledgecount = entity.totaledgecount - 1; //***![check later]
			
			if(prevcptr != NAv){ ll_edge_block_array[prevcptr].metadata.nextcptr = NAv; } //****
			
			// pop-out, pop-in
			if(localbaseaddr >= ll_edge_block_array.size()){ cout<<"bug! : addr out-of-range7 (gds_ll) "<<endl; }
			if(localaddrofvalidedge >= LLEDGEBLOCKSIZE){ cout<<"bug! : addr out-of-range89. entity.lastlocaladdr : "<<entity.lastlocaladdr<<", LLEDGEBLOCKSIZE : "<<LLEDGEBLOCKSIZE<<" (gds_ll) "<<endl; }
			if(ll_localbaseaddrptr >= ll_edge_block_array.size()){ cout<<"bug! : addr out-of-range9 (gds_ll) "<<endl; }
			if(ll_localaddrptr >= LLEDGEBLOCKSIZE){ cout<<"bug! : addr out-of-range10. ll_localaddrptr : "<<ll_localaddrptr<<", LLEDGEBLOCKSIZE : "<<LLEDGEBLOCKSIZE<<" (gds_ll) "<<endl; }
			
			// pop-out
			nedge = ll_edge_block_array[localbaseaddr].ll_edgeblock[localaddrofvalidedge];
			
			// invalidate //***
			if(nedge.flag != VALID){ cout<<"bug! : something wrong552. localbaseaddr : "<<localbaseaddr<<", localaddrofvalidedge : "<<localaddrofvalidedge<<" (gds_ll) "<<endl; }
			ll_edge_block_array[localbaseaddr].ll_edgeblock[localaddrofvalidedge].flag = INVALID;
			
			// pop in
			if(ll_edge_block_array[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr].flag == VALID){ cout<<"bug! : something wrong557. ll_localbaseaddrptr : "<<ll_localbaseaddrptr<<", ll_localaddrptr : "<<ll_localaddrptr<<" (gds_ll) "<<endl; }
			ll_edge_block_array[ll_localbaseaddrptr].ll_edgeblock[ll_localaddrptr] = nedge;
			
			// redirect pointer (heba -> ll)
			unsigned int workblockaddr = get_edgeblock_offset(nedge.heba_hvtx_id) + nedge.heba_workblockid;
			if(nedge.which_gen_is_the_main_copy_located == 1){
				if(workblockaddr >= edge_block_array_m.size()){ cout<<"graphtinker::ll_functions : addr out-of-range 96 (gds_ll) "<<endl; }
				edge_block_array_m[workblockaddr].edges[nedge.heba_loffset].ll_localbaseaddrptr = ll_localbaseaddrptr;
				edge_block_array_m[workblockaddr].edges[nedge.heba_loffset].ll_localaddrptr = ll_localaddrptr;
			} else {
				if(workblockaddr >= edge_block_array_c.size()){ cout<<"graphtinker::ll_functions : addr out-of-range 97 (gds_ll) "<<endl; }
				edge_block_array_c[workblockaddr].edges[nedge.heba_loffset].ll_localbaseaddrptr = ll_localbaseaddrptr;
				edge_block_array_c[workblockaddr].edges[nedge.heba_loffset].ll_localaddrptr = ll_localaddrptr;
			}
			
			// update
			ll_lva[lvaindex] = entity;
			return;
		}
	} else {
		cout<<"bug! : should never get here45 (gds_ll)"<<endl;
	}
	return;
}

void graphtinker::ll_updateedgeptrs(
			edge_t edge,
			moduleparams_t moduleparams,
			vector<ll_edgeblock_t> & ll_edge_block_array
			){
	if(edge.heba_hvtx_id < 0){ cout<<"bug! : invalid454. edge.heba_hvtx_id : "<<edge.heba_hvtx_id<<" (gds_ll) "<<endl; }	
	if(moduleparams.ll_localbaseaddrptr_x >= ll_edge_block_array.size()){ cout<<"bug! : addr out-of-range47 (gds_ll) "<<endl; }
	if(moduleparams.ll_localaddrptr_x >= LLEDGEBLOCKSIZE){ cout<<"bug! : addr out-of-range5. ll_localaddrptr : "<<moduleparams.ll_localaddrptr_x<<", LLEDGEBLOCKSIZE : "<<LLEDGEBLOCKSIZE<<" (gds_ll) "<<endl; }
	ll_edge_block_array[moduleparams.ll_localbaseaddrptr_x].ll_edgeblock[moduleparams.ll_localaddrptr_x].heba_hvtx_id = edge.heba_hvtx_id;
	ll_edge_block_array[moduleparams.ll_localbaseaddrptr_x].ll_edgeblock[moduleparams.ll_localaddrptr_x].heba_workblockid = edge.heba_workblockid;
	ll_edge_block_array[moduleparams.ll_localbaseaddrptr_x].ll_edgeblock[moduleparams.ll_localaddrptr_x].heba_loffset = edge.heba_loffset;
	return; 
}
#endif