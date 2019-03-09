#ifndef GRAPHTINKER_H
#define GRAPHTINKER_H

#include <iostream>
#include <vector>
#include "../common/common.h"
#include "../vertices/vertices.h" 
using namespace std;

/// classes
class graphtinker{
	public:                     
		graphtinker(unsigned int id);
		~graphtinker();		

		// edge block array
		vector<edge_nt> edge_block_array;
		tracker_t lvatracker;
		
		// ll edge block array
		ll_edgeblock_t * ll_edge_block_array;
		ll_logicalvertexentity_t * ll_lva;
		ll_eba_tracker_t ll_eba_tracker;
		
		// vertices 
		vertices vertices_handler;
		vertex_translator_t * vertex_translator;
		tracker_t translator_tracker;
		
		// metadata (for delete and crumple in)
		#ifdef EN_CRUMPLEINONDELETE	
		vector<supervertex_t> svs;
		vector<vertexid_t> freed_edgeblock_list;
		#endif
		vector<edgeblock_parentinfo_t> edgeblock_parentinfo;
		
		// member functions
		void insert_edge(unsigned int src, unsigned int dst, unsigned int ew);
		void delete_edge(unsigned int src, unsigned int dst, unsigned int ew);
		void update_edge(unsigned int src, unsigned int dst, unsigned int ew, unsigned int edgeupdatecmd);
		vertexid_t retrieve_edges(vertexid_t vid, vector<edge_tt> & edges);
		vector<edge_nt> & get_edge_block_array();
		unsigned int printv_edgecount();
		unsigned int printv_uniqueedgecount();
		unsigned int printll_uniqueedgecount();
		unsigned int ll_countuniqueedges(ll_edgeblock_t * ll_edge_block_array);
		unsigned int print_svs_size();
		unsigned int print_freed_edgeblock_list_size();
		void initialize_lvas();
		void iterator();
		vertexid_t get_localvid(vertexid_t globalvid);
		tracker_t get_translator_tracker();
		vertices & get_vertices_handler();
	private:
};

/// function declarations
// commonutility
unsigned int max(unsigned int A, unsigned int B);

// compute unit 
void compute_unit(
				moduleunitcmd_t moduleunitcmd,
				moduleparams_t * moduleparams,
				insertparams_t * insertparams, 
				insertreport_t * insertreport,		
				findparams_t findparams,
				findreport_t * findreport,				
				margin_t wblkmargin, 
				margin_t subblkmargin,
				margin_t start_wblkmargin,
				bucket_t hadjvtx_id, 
				edge_nt * edgeblock,
				edge_t * edge,
				vertexid_t xvtx_id,
				unsigned int rolledover,
				unsigned int edgeupdatecmd
				);
				
// eba_manager
unsigned int get_edgeblock_offset(vertexid_t vid);

// find margin 
void findwblkmargin(margin_t *blkmargin, bucket_t hadjvtx_id);
void findsubblkmargin(margin_t *subblkmargin, bucket_t hadjvtx_id);
unsigned int findsubblkId(vertexid_t vid);

// find unit 
void find_unit(
			moduleparams_t * moduleparams,
			findparams_t findparams,
			findreport_t * findreport,				
			margin_t wblkmargin, 
			margin_t subblkmargin,
			bucket_t hadjvtx_id, 
			edge_nt * edgeblock,
			unsigned int edgeupdatecmd);
			
// gds_utility
unsigned int getsvtracker(markertracker_t * svtracker);

// hash gen 
bucket_t googlehash(vertexid_t vid, unsigned int geni);

// inference unit 
void inference_unit(
				unsigned int edgeupdatecmd,
				moduleunitcmd_t * moduleunitcmd,
				moduleparams_t * moduleparams,				
				loadunitcmd_t * loadunitcmd,
				findparams_t findparams,
				findreport_t findreport,
				insertparams_t insertparams, //***
				insertreport_t insertreport,
				writebackunitcmd_t * writebackunitcmd,				
				intervalunitcmd_t * intervalunitcmd,				
				margin_t * wblkmargin, 
				margin_t subblkmargin,
				margin_t start_wblkmargin,				
				vertexid_t xvtx_id
				#ifdef EN_LLGDS
				,llgdsunitcmd_t *llgdsunitcmd
				#endif
				#ifdef EN_CRUMPLEINONDELETE
				,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
				#endif
				);				
void findonlymode_searchstopped_searchunsuccessful_x_x(
			moduleunitcmd_t *moduleunitcmd,	
			moduleparams_t *moduleparams,
			loadunitcmd_t *loadunitcmd,
			findparams_t findparams,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void findonlymode_searchstopped_searchsuccessful_x_x(
			moduleunitcmd_t *moduleunitcmd,
			moduleparams_t *moduleparams, //***
			loadunitcmd_t *loadunitcmd,
			findparams_t findparams,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void findonlymode_searchnotstopped_searchnotsuccessful_x_notlastworkblock(
			moduleunitcmd_t *moduleunitcmd,
			moduleparams_t *moduleparams,			
			loadunitcmd_t *loadunitcmd,
			findparams_t findparams,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t *wblkmargin, 
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void findonlymode_searchnotstopped_searchnotsuccessful_x_lastworkblock(
			moduleunitcmd_t *moduleunitcmd,
			moduleparams_t *moduleparams, //***
			loadunitcmd_t *loadunitcmd,
			findparams_t findparams,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void insertonlymode_x_x_passedthroughandswappedatsomepoint_notlastworkblock(
			moduleunitcmd_t *moduleunitcmd,
			moduleparams_t *moduleparams,	//***		
			loadunitcmd_t *loadunitcmd,
			insertparams_t insertparams,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t *wblkmargin, 
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void insertonlymode_x_x_passedthroughandswappedatsomepoint_lastworkblock(
			moduleunitcmd_t *moduleunitcmd,
			moduleparams_t *moduleparams,		
			loadunitcmd_t *loadunitcmd,
			insertparams_t insertparams, //***
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void insertonlymode_x_x_loadedintoemptybucket_x(
			moduleunitcmd_t *moduleunitcmd,
			moduleparams_t *moduleparams,
			loadunitcmd_t *loadunitcmd,
			insertparams_t insertparams, //***
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void insertonlymode_x_x_foundandupdateditself_x(
			moduleunitcmd_t *moduleunitcmd,	
			moduleparams_t *moduleparams,
			loadunitcmd_t *loadunitcmd,
			insertparams_t insertparams, //***
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void insertonlymode_x_x_passedthrough_notlastworkblock(
			moduleunitcmd_t *moduleunitcmd,
			moduleparams_t *moduleparams,			
			loadunitcmd_t *loadunitcmd,
			insertparams_t insertparams, //***
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t *wblkmargin, 
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void insertonlymode_x_x_passedthrough_lastworkblock(
			moduleunitcmd_t *moduleunitcmd,
			moduleparams_t *moduleparams,			
			loadunitcmd_t *loadunitcmd,
			insertparams_t insertparams, //***
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void findanddeletemode_searchstopped_searchunsuccessful_x_x(
			moduleunitcmd_t *moduleunitcmd,	
			moduleparams_t *moduleparams,
			loadunitcmd_t *loadunitcmd,
			findparams_t findparams,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void findanddeletemode_searchstopped_searchsuccessful_x_x(
			moduleunitcmd_t *moduleunitcmd,	
			moduleparams_t *moduleparams,
			loadunitcmd_t *loadunitcmd,
			findparams_t findparams,
			findreport_t findreport,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void findanddeleteandcrumpleinmode_searchstopped_searchunsuccessful_x_x(
			moduleunitcmd_t *moduleunitcmd,	
			moduleparams_t *moduleparams,
			loadunitcmd_t *loadunitcmd,
			findparams_t findparams,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);
void findanddeleteandcrumpleinmode_searchstopped_searchsuccessful_x_x(
			moduleunitcmd_t *moduleunitcmd,	
			moduleparams_t *moduleparams,
			loadunitcmd_t *loadunitcmd,
			findparams_t findparams,
			findreport_t findreport,
			writebackunitcmd_t *writebackunitcmd,
			intervalunitcmd_t *intervalunitcmd,
			margin_t wblkmargin,
			margin_t subblkmargin,
			vertexid_t xvtx_id
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_popoutpopin_cmd
			#endif
);

void set_intervalunitcmd_continue_from_first_generation(intervalunitcmd_t *intervalunitcmd);
void set_intervalunitcmd_continue_in_lower_generation(intervalunitcmd_t *intervalunitcmd);
void set_intervalunitcmd_continue_in_current_generation(intervalunitcmd_t *intervalunitcmd, margin_t *wblkmargin, margin_t subblkmargin, moduleparams_t *moduleparams);
void set_intervalunitcmd_quit_to_next_edge(intervalunitcmd_t *intervalunitcmd);
void set_loadunitcmd_loadnextEB(loadunitcmd_t *loadunitcmd, unsigned int cmd);
void set_moduleunitcmd(moduleunitcmd_t *moduleunitcmd, unsigned int cmd);
void set_moduleunitparamsedgefields(moduleparams_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight);
void set_writebackunitcmd_writebackcurrentEB(writebackunitcmd_t *writebackunitcmd, unsigned int cmd, unsigned int addr, margin_t subblkmargin);
void setwritebackunitcmd_markasclustered(writebackunitcmd_t *writebackunitcmd, unsigned int cmd, unsigned int addr);
void set_llgdsunitcmd(llgdsunitcmd_t *llgdsunitcmd, unsigned int cmd);
void updatemarginandrolloverstatus(margin_t *wblkmargin, margin_t subblkmargin, moduleparams_t *moduleparams);
unsigned int isthelastworkblock(margin_t wblkmargin, margin_t start_wblkmargin, margin_t subblkmargin, moduleparams_t moduleparams);

// init 
unsigned int add_page(tracker_t * tracker);
unsigned int add_page2(tracker_t * tracker, vector<edge_nt> & edge_block_array);

// insert unit 
void insert_unit(
				moduleparams_t *moduleparams,
				insertparams_t *insertparams, 
				insertreport_t *insertreport,
				margin_t wblkmargin, 
				margin_t subblkmargin,
				bucket_t hadjvtx_id, 
				edge_nt * edgeblock,
				edge_t * edge,
				vertexid_t xvtx_id,
				unsigned int rolledover);
bucket_t getdib(bucket_t currbkt, bucket_t initialbucket, margin_t subblkmargin, unsigned int rolledover);

// interval unit 
void interval_unit_updateedge(			
			moduleunitcmd_t * moduleunitcmd,
			moduleparams_t * moduleparams,
			loadunitcmd_t * loadunitcmd,
			insertparams_t * insertparams, 
			insertreport_t * insertreport,		
			findparams_t * findparams,
			findreport_t * findreport,	
			writebackunitcmd_t * writebackunitcmd,
			intervalunitcmd_t intervalunitcmd,				
			margin_t * wblkmargin,
			margin_t * subblkmargin,
			margin_t * start_wblkmargin,
			margin_t * first_wblkmargin,
			vertexid_t * xvtx_id,
			bucket_t * hadjvtx_id,
			edge_t edge,
			unsigned int edgeupdatecmd,
			unsigned int * tripiteration_lcs,
			unsigned int * geni,
			unsigned int * quitstatus,
			unsigned int infiniti
			#ifdef EN_CRUMPLEINONDELETE
			,unsigned int *lastgenworkblockaddr
			#endif
			#ifdef EN_LLGDS
			,llgdsunitcmd_t *llgdsunitcmd
			#endif
			#ifdef EN_CRUMPLEINONDELETE
			,deleteandcrumpleincmd_t * heba_deleteandcrumplein_cmd
			#endif
			);

// interval unit functions 
void initallmoduleunit(moduleunitcmd_t *moduleunitcmd, moduleparams_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight);
void initialize_moduleunit_params(moduleparams_t *moduleparams, vertexid_t xadjvtx_id, edgeweight_t edge_weight);
void initialize_moduleunit_params2(moduleparams_t *moduleparams);
void initialize_loadunit(loadunitcmd_t *loadunitcmd);
void initialize_insertunit(insertparams_t *insertparams, insertreport_t *insertreport, vertexid_t xadjvtx_id, bucket_t hadjvtx_id, edgeweight_t edge_weight);
void initialize_insertunit2(insertparams_t *insertparams, insertreport_t *insertreport);
void initialize_findunit(findparams_t *findparams, findreport_t *findreport, vertexid_t xadjvtx_id, bucket_t hadjvtx_id, edgeweight_t edge_weight);
void initialize_findunit2(findparams_t *findparams, findreport_t *findreport);
void initialize_writebackunit(writebackunitcmd_t *writebackunitcmd);
void initialize_llebaverdictcmd(llgdsunitcmd_t *llgdsunitcmd);
#ifdef EN_LLGDS
void clear_lleba_addresses_in_moduleparams(moduleparams_t *moduleparams);
#endif
void init_deleteandcrumplein_verdictcmd(deleteandcrumpleincmd_t *heba_deleteandcrumplein_cmd);

// ll functions 
void ll_insert(
			edge_t edge,
			moduleparams_t * moduleparams,
			insertparams_t * insertparams,
			ll_edgeblock_t * ll_edge_block_array,
			ll_logicalvertexentity_t * ll_lva,
			ll_eba_tracker_t * ll_eba_tracker
			);			
void ll_update(
			edge_t edge,
			vertexid_t ll_localbaseaddrptr,
			vertexid_t ll_localaddrptr,
			ll_edgeblock_t *ll_edge_block_array
			);			
void ll_delete(
			edge_t edge,
			vertexid_t ll_localbaseaddrptr,
			vertexid_t ll_localaddrptr,
			ll_edgeblock_t * ll_edge_block_array
			);			
void ll_deleteandcrumplein(
			edge_t edge,
			vertexid_t ll_localbaseaddrptr,
			vertexid_t ll_localaddrptr,
			ll_edgeblock_t * ll_edge_block_array,
			ll_logicalvertexentity_t * ll_lva,
			ll_eba_tracker_t * ll_eba_tracker,
			vector<edge_nt> & edge_block_array
			);			
void ll_updateedgeptrs(
			edge_t edge,
			moduleparams_t moduleparams,
			ll_edgeblock_t * ll_edge_block_array
			);

// load parameters 
void load_params(
				moduleparams_t moduleparams,				
				insertparams_t *insertparams,		
				findparams_t *findparams,				
				margin_t wblkmargin, 
				bucket_t hadjvtx_id
				);

// load unit 
void load_unit(				
			moduleparams_t *moduleparams,
			loadunitcmd_t loadunitcmd,				
			insertparams_t *insertparams, 		
			findparams_t *findparams,
			margin_t wblkmargin,
			vertexid_t hvtx_id,
			edge_t edge,
			edge_nt *edgeblock,
			vector<edge_nt> & edge_block_array,
			unsigned int * prevLoadAddr
			);

// super vertices 
int sv_get_next_edge(
			unsigned int xvtx_id,
			vector<supervertex_t> &svs,
			vector<vertexid_t> & freed_edgeblock_list,
			edge_tt * edgett,
			margin_t wblkmargin,
			writebackunitcmd_t writebackunitcmd,
			int * tailhvtx_id,
			unsigned int * svs_index,
			unsigned int * numclusteredworkblocks,
			unsigned int geni,
			vector<edge_nt> & edge_block_array
			);
int sv_get_tail_edgeblock(
			vector<supervertex_t> & svs,
			unsigned int currworkblockaddr,
			unsigned int *svs_index,
			unsigned int geni,
			vector<edge_nt> & edge_block_array 
			);
int sv_pick_edge(
			unsigned int xvtx_id,
			unsigned int offset,
			edge_tt * edgett,
			unsigned int * numclusteredworkblocks,
			vector<edge_nt> & edge_block_array
			);

// utility
unsigned int bitcombine_AB(unsigned int A, unsigned int B);
unsigned int bitselect_ABC(unsigned int A, unsigned int B, unsigned int C);
float min_float(float A, float B);
vertexdata_t min_type1(vertexdata_t A, vertexdata_t B);
unsigned int getpartitionid(unsigned int vid);

// vertices
vertexproperty_t readproperty(vertexid_t vertexid);
void writeproperty(vertexid_t vertexid, vertexdata_t vertexdata);
void update_vertex_property(edge_t edge, unsigned int edgedirection, unsigned int edgeupdatecmd);
void initialize();

// writeback unit 
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
		);
				
// delete and crumple in unit 
void deleteandcrumplein_unit(
			writebackunitcmd_t writebackunitcmd,
			findreport_t findreport,
			edge_t edge,
			vector<edge_nt> & edge_block_array,
			#ifdef EN_LLGDS
			ll_edgeblock_t * ll_edge_block_array,
			#endif
			vector<edgeblock_parentinfo_t> & edgeblock_parentinfo,
			vertexid_t xvtx_id,
			margin_t wblkmargin,
			margin_t subblkmargin,
			unsigned int geni
			,deleteandcrumpleincmd_t deleteandcrumpleincmd
			,vector<supervertex_t> & svs
			,vector<vertexid_t> & freed_edgeblock_list
			);
			
// ll unit 
void ll_unit(
		llgdsunitcmd_t llgdsunitcmd,
		moduleparams_t * moduleparams,
		insertparams_t * insertparams,
		edge_t edge,
		vector<edge_nt> & edge_block_array
		#ifdef EN_LLGDS
		,ll_edgeblock_t * ll_edge_block_array
		,ll_logicalvertexentity_t * ll_lva
		,ll_eba_tracker_t * ll_eba_tracker
		#endif
		);
#endif








