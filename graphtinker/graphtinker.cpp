#include <string.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <functional>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include "../vertices/vertices.h"
#include "graphtinker.h"
using namespace std;

graphtinker::graphtinker(unsigned int id){
	// edge block array
	edge_block_array.resize(EDGEBLOCKARRAYSIZE);
	lvatracker.mark = EDGEBLOCKARRAYHEIGHT;
	
	// ll edge block array
	#ifdef EN_LLGDS
	ll_edge_block_array = new ll_edgeblock_t[LLEDGEBLOCKARRAYSIZE];
	ll_lva = new ll_logicalvertexentity_t[LLLOGICALVERTEXARRAYSIZE];
	ll_eba_tracker.ptraddr=0;	
	initialize_lvas();
	#endif
	
	// vertices & translator
	vertex_translator = new vertex_translator_t[NO_OF_VERTICES];
	translator_tracker.mark = 0;
	
	// metadata (for delete and crumple in)
	edgeblock_parentinfo.resize(261875);
}
graphtinker::~graphtinker(){
	// printf("graphtinker destructor called \n");
	/* edge_block_array.clear();
	delete [] vertex_translator;
	#ifdef EN_LLGDS	
	delete [] ll_edge_block_array;
	delete [] ll_lva;
	#endif */
}

void graphtinker::insert_edge(unsigned int src, unsigned int dst, unsigned int ew){
	unsigned int edgeupdatecmd = INSERTEDGE;	
	#ifdef EN_SGHASHING
	vertexid_t local_srcvid = get_localvid((vertexid_t)src);
	vertexid_t local_dstvid = get_localvid((vertexid_t)dst);
	src = local_srcvid;
	dst = local_dstvid;
	#endif	
	// cout<<"src : "<<src<<", dst : "<<dst<<endl;
	update_edge(src, dst, ew, edgeupdatecmd);
	return;
}

void graphtinker::delete_edge(unsigned int src, unsigned int dst, unsigned int ew){
	unsigned int edgeupdatecmd = DELETEEDGE;
	#ifdef EN_SGHASHING
	vertexid_t local_srcvid = get_localvid((vertexid_t)src);
	vertexid_t local_dstvid = get_localvid((vertexid_t)dst);
	src = local_srcvid;
	dst = local_dstvid;
	#endif
	update_edge(src, dst, ew, edgeupdatecmd);
	return;
}

void graphtinker::update_edge(unsigned int src, unsigned int dst, unsigned int ew, unsigned int edgeupdatecmd){
	// create an edge object
	edge_t edge;
	edge.xvtx_id = src;								
	edge.xadjvtx_id = dst;
	edge.edgew = 1; 
	edge.flag = VALID;
	#ifdef EN_LLGDS
	edge.heba_hvtx_id = -1;
	edge.heba_workblockid = -1;
	edge.heba_loffset = -1;
	#endif
	
	// variables & parameter registers and RAM declared
	unsigned int eit;
	unsigned int geni;
	vertexid_t xvtx_id;
	unsigned int tripiteration_lcs=0;		

	///////////////////////conversion unit  
	//hash xadjvtx_id 
	bucket_t hadjvtx_id=0;  

	///////////////////////edgeblock
	edge_nt edgeblock;

	///////////////////////margins & traversal trackers
	margin_t wblkmargin;
	margin_t start_wblkmargin;
	margin_t first_wblkmargin;
	margin_t subblkmargin; 

	///////////////////////params and report
	//module parameters and reports
	moduleunitcmd_t moduleunitcmd;
	moduleparams_t moduleparams;
	
	//load unit 
	loadunitcmd_t loadunitcmd;

	//insert-unit parameters and reports
	insertparams_t insertparams;
	insertreport_t insertreport;  
	
	//find-unit parameters and reports
	findparams_t findparams;
	findreport_t findreport;
	searchreport_t searchreport;
	
	//writeback unit 
	writebackunitcmd_t writebackunitcmd;
	
	//interval unit 
	intervalunitcmd_t intervalunitcmd;
	
	//LL unit 
	#ifdef EN_LLGDS	
	llgdsunitcmd_t llgdsunitcmd;
	#endif
	
	#ifdef EN_CRUMPLEINONDELETE
	deleteandcrumpleincmd_t deleteandcrumpleincmd;
	#endif

	//exit condition (this should be after: 'insert-unit parameters and reports')
	unsigned int quitstatus=0;	
	
	////////////////////////// initial assignments
	// initialize appropriate module globals
	geni = 1;
	xvtx_id = edge.xvtx_id;	
	tripiteration_lcs = 0;
	hadjvtx_id = googlehash(edge.xadjvtx_id, geni);
	findwblkmargin(&wblkmargin, hadjvtx_id); //find margins
	start_wblkmargin = wblkmargin;
	first_wblkmargin = wblkmargin;
	findsubblkmargin(&subblkmargin, hadjvtx_id);
	#ifdef EN_CRUMPLEINONDELETE
	// keep track of workblock address before moving to a lower generation (or in first launch) -  might be needed in the lower generation if we decide to share supervertex entry
	unsigned int lastgenworkblockaddr = get_edgeblock_offset(xvtx_id) + wblkmargin.top/WORK_BLOCK_HEIGHT;
	#endif

	//initialize module unit 
	moduleunitcmd.mode=FINDONLYMODE;
		
	// initialize appropriate fields of lcs units
	// everything is initialized here
	initialize_moduleunit_params(&moduleparams, edge.xadjvtx_id, edge.edgew);
	initialize_loadunit(&loadunitcmd);
	initialize_insertunit(&insertparams, &insertreport, edge.xadjvtx_id, hadjvtx_id, edge.edgew);
	initialize_findunit(&findparams, &findreport, edge.xadjvtx_id, hadjvtx_id, edge.edgew);
	initialize_writebackunit(&writebackunitcmd);
	#ifdef EN_LLGDS		
	initialize_llebaverdictcmd(&llgdsunitcmd);
	clear_lleba_addresses_in_moduleparams(&moduleparams);
	#endif
	#ifdef EN_CRUMPLEINONDELETE
	init_deleteandcrumplein_verdictcmd(&deleteandcrumpleincmd);
	#endif

	//initialize trip iteration
	unsigned int infiniti=0;
	unsigned int prevLoadAddr=NAv;
	
	#ifdef EN_UPDATEVERTEXPROPS
	// update vertex of first edge
	vertices_handler.update_vertex_property(edge, edgeupdatecmd);
	#endif

	//***
	//***
	//***
	//***
	//***
	//***
	//***
	//*** loop
	for(infiniti=0; infiniti<SOMELARGENO; infiniti++){
		
		//***********************************************************************************************//Load unit
		load_unit(
				&moduleparams,
				loadunitcmd,				
				&insertparams, 		
				&findparams,
				wblkmargin,
				xvtx_id,
				edge,
				&edgeblock,
				edge_block_array,
				&prevLoadAddr		
				);
		//***********************************************************************************************
		
		//***********************************************************************************************//load compute-params unit
		load_params(
					moduleparams,				
					&insertparams,		
					&findparams,
					wblkmargin, 
					hadjvtx_id
					);
		//***********************************************************************************************
		
		//***********************************************************************************************//Compute unit
		/// one-time-fields of incomming edge is updated here
		compute_unit(				
					moduleunitcmd,
					&moduleparams,
					&insertparams, 
					&insertreport,		
					findparams,
					&findreport,				
					wblkmargin, 
					subblkmargin,
					start_wblkmargin,
					hadjvtx_id, 
					&edgeblock,
					&edge,
					xvtx_id,
					moduleparams.rolledover,
					edgeupdatecmd
					);
		//***********************************************************************************************
		
		//***********************************************************************************************//Inference unit
		inference_unit(
					edgeupdatecmd,
					&moduleunitcmd,
					&moduleparams,				
					&loadunitcmd,
					findparams,
					findreport,
					insertparams,
					insertreport,				
					&writebackunitcmd,				
					&intervalunitcmd,				
					&wblkmargin, 
					subblkmargin,
					start_wblkmargin,				
					xvtx_id
					#ifdef EN_LLGDS	
					,&llgdsunitcmd
					#endif
					#ifdef EN_CRUMPLEINONDELETE
					,&deleteandcrumpleincmd
					#endif
		);
		//***********************************************************************************************
		
		//***********************************************************************************************//EBA port unit
		writeback_unit(
					&moduleparams,				
					writebackunitcmd,
					edge,
					&edgeblock,
					edge_block_array,
					edgeblock_parentinfo,
					&lvatracker,
					xvtx_id,
					first_wblkmargin,
					subblkmargin,
					geni,
					edgeupdatecmd
					#ifdef EN_CRUMPLEINONDELETE
					,lastgenworkblockaddr
					,svs
					#endif
					);
		//***********************************************************************************************

		//***********************************************************************************************// delete and crumple in
		#ifdef EN_CRUMPLEINONDELETE
		deleteandcrumplein_unit(
			writebackunitcmd,
			findreport,
			edge,
			edge_block_array,
			#ifdef EN_LLGDS
			ll_edge_block_array,
			#endif
			edgeblock_parentinfo,
			xvtx_id,
			wblkmargin,
			subblkmargin,
			geni
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd
			,svs
			,freed_edgeblock_list
			#endif
			);
		#endif
		//***********************************************************************************************
	
		//***********************************************************************************************// GraphTinkerLL
		#ifdef EN_LLGDS
		ll_unit(
			llgdsunitcmd,
			&moduleparams,
			&insertparams,
			edge,
			edge_block_array
			#ifdef EN_LLGDS
			,ll_edge_block_array
			,ll_lva
			,&ll_eba_tracker
			#endif
			);
		#endif
		//***********************************************************************************************
		
		//***********************************************************************************************//Interval unit
		interval_unit_updateedge(			
					&moduleunitcmd,
					&moduleparams,
					&loadunitcmd,
					&insertparams, 
					&insertreport,		
					&findparams,
					&findreport,
					&writebackunitcmd,
					intervalunitcmd,				
					&wblkmargin,
					&subblkmargin,
					&start_wblkmargin,
					&first_wblkmargin,
					&xvtx_id,
					&hadjvtx_id,
					edge,
					edgeupdatecmd,
					&tripiteration_lcs,
					&geni,
					&quitstatus,
					infiniti
					#ifdef EN_CRUMPLEINONDELETE
					,&lastgenworkblockaddr
					#endif
					#ifdef EN_LLGDS	
					,&llgdsunitcmd
					#endif
					#ifdef EN_CRUMPLEINONDELETE
					,&deleteandcrumpleincmd
					#endif
					);
		
		//***********************************************************************************************
		
		//***********************************************************************************************// Update Vertex Property (e.g degrees, etc)
		#ifdef EN_UPDATEVERTEXPROPS
		vertices_handler.update_vertex_property(edge, edgeupdatecmd);
		#endif 		
		//***********************************************************************************************
		
		if(quitstatus==1){ 
			#ifdef cpuem_l7
			cout<<endl<<"successful. exiting... exiting... (update_singleedge)"<<endl; 
			#endif 
			return;
		} 
		if(tripiteration_lcs>100){
			#ifdef cpuem_bugs_b1
			cout<<endl<<"bug! : trip iteration too much!, exiting..."<<endl; 
			#endif 
			return; 
		}
		if(geni>500000){
			#ifdef cpuem_bugs_b1
			cout<<"bug! : too many generations ("<<geni<<") spun!, exiting..."<<endl;  
			cout<<"searchreport.searchstop = "<<searchreport.searchstop<<endl;
			cout<<"searchreport.searchsuccessful = "<<searchreport.searchsuccessful<<endl;
			cout<<"insertreport.exittype = "<<insertreport.exittype<<endl;
			cout<<"moduleunitcmd.mode = "<<moduleunitcmd.mode<<endl;
			#endif
			return; 
		}
	}
	cout<<endl<<"bug! : infiniti exhausted : infiniti = "<<infiniti<<""<<endl;
	cout<<"searchreport.searchstop = "<<searchreport.searchstop<<endl;
	cout<<"searchreport.searchsuccessful = "<<searchreport.searchsuccessful<<endl;
	cout<<"insertreport.exittype = "<<insertreport.exittype<<endl;
	cout<<"moduleunitcmd.mode = "<<moduleunitcmd.mode<<endl;
	return; 
}

vertexid_t graphtinker::retrieve_edges(vertexid_t vid, vector<edge_tt> & edges){		
	vector<clusterptr_t> clusterptrs;	
	vertexid_t basevid = vid;
	
	// load edges
	clusterptrs.push_back(vid);
	unsigned int len = clusterptrs.size();	
	unsigned int wps = WORK_BLOCKS_PER_SUBBLOCK;
	
	while(true){
		for(unsigned int i=0; i<len; i++){		
			vid = clusterptrs.back();
			clusterptrs.pop_back();
			
			unsigned int ebaoffset = get_edgeblock_offset(vid);
			for(unsigned int t=0; t<WORK_BLOCKS_PER_PAGE; t++){
				edge_nt edgeset = edge_block_array[(ebaoffset + t)];
				
				for(unsigned int k=0; k<WORK_BLOCK_HEIGHT; k++){
					if(edgeset.edges[k].flag == VALID){
						edges.push_back(edgeset.edges[k]);
					}
				}
				
				if(((t%wps)==0) && (edgeset.clusterinfo.flag==VALID)){
					clusterptrs.push_back(edgeset.clusterinfo.data); 
				}
			}
		}
	
		len = clusterptrs.size();
		if(len==0){ break; }
	}
	return basevid;
}

vector<edge_nt> & graphtinker::get_edge_block_array(){
	return edge_block_array;
}

unsigned int graphtinker::printv_edgecount(){
	unsigned int edgecount = 0;
	for(vertexid_t vid=0; vid<lvatracker.mark; vid++){
		unsigned int offset = get_edgeblock_offset(vid);		
		for(unsigned int j=offset; j<(offset+WORK_BLOCKS_PER_PAGE); j++){ 
			for(unsigned int k=0; k<WORK_BLOCK_HEIGHT; k++){
				if (edge_block_array[j].edges[k].flag == VALID){ edgecount += 1; }
			}
		}
	}
	return edgecount;
}

unsigned int graphtinker::printv_uniqueedgecount(){
	unsigned int edgecount = 0;
	for(vertexid_t vid=0; vid<lvatracker.mark; vid++){
		unsigned int offset = get_edgeblock_offset(vid);		
		for(unsigned int j=offset; j<(offset+WORK_BLOCKS_PER_PAGE); j++){ 
			for(unsigned int k=0; k<WORK_BLOCK_HEIGHT; k++){
				if (edge_block_array[j].edges[k].flag == VALID){ edgecount += edge_block_array[j].edges[k].edge_weight; }
			}
		}
	}
	return edgecount;
}

unsigned int graphtinker::printll_uniqueedgecount(){
	unsigned int uniqueedgecount = 0;
	uniqueedgecount += ll_countuniqueedges(ll_edge_block_array);
	return uniqueedgecount;
}

unsigned int graphtinker::ll_countuniqueedges(ll_edgeblock_t * ll_edge_block_array){
	unsigned int uniqueedgecount = 0;
	for(unsigned int i=0;i<LLEDGEBLOCKARRAYSIZE;i++){
		for(unsigned int j=0;j<LLEDGEBLOCKSIZE;j++){
			if(ll_edge_block_array[i].ll_edgeblock[j].flag==VALID){ uniqueedgecount += ll_edge_block_array[i].ll_edgeblock[j].edgew; }
		}
	}	
	return uniqueedgecount;
}

#ifdef EN_CRUMPLEINONDELETE	
unsigned int graphtinker::print_svs_size(){
	return svs.size();
}
#endif

#ifdef EN_CRUMPLEINONDELETE	
unsigned int graphtinker::print_freed_edgeblock_list_size(){
	return freed_edgeblock_list.size();
}
#endif

void graphtinker::initialize_lvas(){
	for(unsigned int i=0;i<LLLOGICALVERTEXARRAYSIZE;i++){	
		ll_lva[i].baseaddr=0;
		ll_lva[i].lastlocalbaseaddr=0;
		ll_lva[i].lastlocaladdr=0;
		ll_lva[i].totaledgecount=0;
		ll_lva[i].flag = INVALID;
	}
	return;
}

vertexid_t graphtinker::get_localvid(vertexid_t globalvid){
	if(globalvid > NO_OF_VERTICES){ cout<<"bug. out of range5. globalvid : "<<globalvid<<", NO_OF_VERTICES : "<<NO_OF_VERTICES<<" (get_localvid)"<<endl; }
	if(vertex_translator[globalvid].flag != VALID){
		vertex_translator[translator_tracker.mark].globalvid = globalvid;
		vertex_translator[globalvid].localvid = translator_tracker.mark;
		translator_tracker.mark += 1;
		vertex_translator[globalvid].flag = VALID;
	}
	return vertex_translator[globalvid].localvid;
}

unsigned int graphtinker::get_translator_tracker(){
	return translator_tracker.mark;
}

vertices & graphtinker::get_vertices_handler(){
	return vertices_handler;
}