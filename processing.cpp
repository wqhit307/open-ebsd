/**
	\file processing.cpp
	\brief Functions for cleaning and processing dataset
*/

#include "ebsd.h"

/// Dilate all grains in dataset until all dropped voxels are filled
int Dataset::dilate_grains()
{
	Point *d, *nn;
	int i,l,k, v, assigned = 0, grain_assigned = 0; 
	
	int x = 1,
		y = tx,
		z = tx*ty;
	
	int n, temp;
	
	int num_nn, max_matched = 0;

	int nn_offset_edge[NUM_NN+1];
	int *nn_offset;

	Grain *grain;

	class GrainList *list;
	int list_end_i;

	bool *grain_finished;
	grain_finished = (bool*) malloc( num_grains * sizeof(bool) );
	if (grain_finished==NULL) { return 0; }

	int *order;
	order = (int*) malloc( num_grains * sizeof(int) );
	if (order==NULL) { return 0; }

	for(i=0;i<num_grains;i++){
		grain_finished[i] = false;
		order[i] = i;
	}
	
	// Cycle through grains until no more voxels can be assigned
	//--------------------------------------------------
	do{
		assigned = 0;
		
		// Randomise order of grains
		//--------------------------------------------------
		srand ( time(NULL) );
		//randomise order
	 	n = num_grains;        // The number of items left to shuffle (loop invariant).
        while (n > 0) 
        {
            k = rand() % n;  	 // 0 <= k < n.
            --n;                 // n is now the last pertinent index;
            temp = order[n];     // swap array[n] with array[k].
            order[n] = order[k];
            order[k] = temp;
        }

		// Cycle through grains which have not been completed
		//--------------------------------------------------
		for(i=0;i<num_grains;i++){
			if (!grain_finished[order[i]]) {
				grain_assigned = 0;
				
				grain = grains+order[i];
				
				if (i%100 == 0) {
					progress_callback((float)(i+1)/num_grains,"Dilating grains", false);
				}
				v = grain->volume;
				k=0;
				list = grain->list_head;
				list_end_i = grain->list_tail->index;
				
				// Cycle through voxels in grain and see if any have neighbours to exand to
				// Make sure not to cycle through grains we've just added
				//--------------------------------------------------
				do{
					d = data + list->index;
					k++;
					nearest_neigbours(list->index,&nn_offset,nn_offset_edge,&num_nn);
					
					//cycle nearest neighbours
					for(l=1;l<num_nn;l++){
						nn = d + nn_offset[l];
						
						if ( !nn->external && nn->grain==-1 ) {
								
							nn->grain = order[i];
							nn->mad = 0.9;
							nn->orientation = grain->orientation;
							
							grain->volume++;
												
							//Add to grainlist
							grain_list_free->index = list->index + nn_offset[l];
							grain_list_free->next = NULL;
							
							grain->list_tail->next = grain_list_free;
							grain->list_tail = grain_list_free;
							grain_list_free++;
							
							assigned++;
							grain_assigned++;
							
						} //endif check grain
					} //endfor cycle nn
				
					list = list->next;
				} while( list!=NULL && k<v );	
				
				if (grain_assigned == 0) {
					grain_finished[order[i]] = true;
				}
			}
		}
//		disp_progress("Dilating grains", 1);
		printf("assigned %i\n",assigned);
	}while (assigned > 0);
	
	free(grain_finished);
	free(order);
	
};


/// Filter grain which contain fewer than critical_volume voxels
/**
	@param critical_volume threshold volume
*/
int Dataset::filter_grains(int critical_volume)
{
	Point *d;
	int i,j, unassigned; 
	
	int max_matched = 0;
	
	bool finished = false;
	GrainList *list;
	Grain *g;
	
	bool *reassigned = NULL;
	reassigned = (bool*) malloc( num_grains*sizeof(bool) );
	if (reassigned == NULL) { return 0; }
		
	for(i=0;i<num_grains;i++){
		reassigned[i] = false;
	}
		
	int max_matches, max_grain, matches, new_grain;
	
	//Merge grains below critial volume to neighbour with largest joint area
	//--------------------------------------------------
	while(!finished){
		unassigned = 0;
		finished = true;
		for(i=0;i<num_grains;i++){
			if ((!reassigned[i]) && grains[i].volume < critical_volume) {
				max_matches = 0;
				max_grain = -1;
				
				for(j=0;j<grains[i].num_adj_grains;j++){
					matches = grains[i].adj_grains[j].nn_matches;
					g = grains + grains[i].adj_grains[j].grain;
					if (matches > max_matches && g->volume > critical_volume) {
						max_matches = matches;
						max_grain = j;
					}
				}
				
				//check that grain has an adjacent grain above critical volume
				//--------------------------------------------------

				if (max_grain>-1) {
					reassigned[i] = true;
					new_grain = grains[i].adj_grains[max_grain].grain;
					for(list = grains[i].list_head; list!=NULL; list=list->next){
						d = data + list->index;
						assert(d->grain == i);
						d->grain = new_grain;
						d->orientation = grains[new_grain].orientation;
					}
					//Add to new grain list
					//--------------------------------------------------
					grains[new_grain].list_tail->next = grains[i].list_head;
					grains[new_grain].list_tail = grains[i].list_tail;
					grains[new_grain].volume = grains[new_grain].volume + grains[i].volume;
					
					//Mark old grain for deletion
					grains[i].list_head = NULL;					 
					grains[i].volume = 0;
					if (grains[i].adj_grains != NULL && grains[i].num_adj_grains > 0) { 		
						free(grains[i].adj_grains);	
					}
					grains[i].adj_grains = NULL;

				} else {
					finished = false;
					unassigned++;
				}
			}
		}

		// Recalculate adjacency list
		//--------------------------------------------------
		printf("Filtered (%i unassigned)\n", unassigned);
	
		adjacency_list();

	}	
	
	free(reassigned);
	
	
	return 1;
}


/// Merge neighbouring grains with disorientation betwen means less than min_angle
/**
	@param min_angle threshold angle for comparison
*/
int Dataset::merge_grains(float min_angle)
{
	Point *d;
	int i,j, unassigned; 
	
	int max_matched = 0;
	
	bool finished = false;
	GrainList *list;

	bool *reassigned = NULL;
	reassigned = (bool*) malloc( num_grains*sizeof(bool) );
	if (reassigned == NULL) { return 0; }
	
	for(i=0;i<num_grains;i++){
		reassigned[i] = false;
	}
	
	int old_grain;
	int merged = 1;
	
	// Keep cycling through the grains until no more can be merged
	//--------------------------------------------------
	while(merged > 0){
		merged = 0;
		unassigned = 0;
		finished = true;
		for(i=0;i<num_grains;i++){
			
			// Check that the grain hasn't already been reassigned
			// and cycle through its nearest neighbours
			//--------------------------------------------------
			if (!reassigned[i]) {
				for(j=0;j<grains[i].num_adj_grains;j++){

					// Merge nearest neighbours which haven't already been reassigned
					// and are below threshold disorientation
					//--------------------------------------------------
					if ( grains[i].adj_grains[j].disorientation < min_angle &&
						 reassigned[grains[i].adj_grains[j].grain]==false) {

						old_grain = grains[i].adj_grains[j].grain;

						merged++;
						reassigned[old_grain] = true;
						for(list = grains[old_grain].list_head; list!=NULL; list=list->next){
							
							d = data + list->index;				
							d->grain = i;
							d->orientation = grains[i].orientation;
						}
						//Add to new grain list
						grains[i].list_tail->next = grains[old_grain].list_head;
						grains[i].list_tail = grains[old_grain].list_tail;
						grains[i].volume = grains[i].volume + grains[old_grain].volume;
						
						//Mark old grain for deletion
						grains[old_grain].list_head = NULL;					 
						grains[old_grain].volume = 0;
						if (grains[old_grain].adj_grains != NULL && grains[old_grain].num_adj_grains > 0) { 		
							free(grains[old_grain].adj_grains);	
						}
						grains[old_grain].adj_grains = NULL;
					
					
					}
				}
			}
		}

		printf("Merged (%i merged)\n",merged);
		// Recalculate adjacency list
		//--------------------------------------------------
		adjacency_list();

	}	
	
	free(reassigned);
	
	
	return 1;
}

/// Interpolate a slice from its neighbours
/**
	@param slice slice to replace
	
	Each point is replaced by the mode orientation of its nearest neighbours in the
	adjacent slices (binned within EULER_TOL).
*/
int Dataset::replace_slice(int slice)
{
	Point *d, *d0, *nn[NUM_NN+1];
	int i,j,k, assigned = 0; 
	
	int nn_match[NUM_NN+1], num_matched = 0, num_nn, max_matched = 0;

	int nn_offset_edge[NUM_NN+1];
	int *nn_offset;

	int start = dz*slice;
	int finish = start + dz;

	if (slice == 0)
		return 0;

	for(i=start;i<finish;i++) {
		d = data + i;
		d0 = d - dz;
	
		d->external = d0->external;
		d->orientation.set_zero();
	
	}

	// Cycle through all points in slice
	//--------------------------------------------------
	for(i=start;i<finish;i++) {
		d = data + i; 
		//Check mad value
		if (!d->external) {

			nearest_neigbours(i,&nn_offset,nn_offset_edge,&num_nn,1);

			for(j=1;j<num_nn;j++) {
				nn_match[j] = 0;
				nn[j] = d+nn_offset[j]; 
			}

			// Bin the orientations of the nearest neighbours
			//--------------------------------------------------
			for(j=1;j<num_nn-1;j++) {
				if (nn[j] != NULL && nn[j]->mad!=0 && !nn[j]->external) {
					for(k=j+1;k<num_nn;k++){
						if (nn[k] != NULL && nn[k]->mad != 0 && !nn[k]->external) {
							//compare euler values
							if ( nn[j]->orientation.disorientation_threshold(&(nn[k]->orientation), QUAT_TOL) ) {					
								nn[k] = NULL;
								nn_match[j]++;
								num_matched++;
							} //endif: comparing
						} //endif: check k
					} //endfor: k
				} //endif: checkok j
			} //endfor: j 
			
			max_matched = 0;
			
			// Determine the mode values
			//--------------------------------------------------
			for(j=1;j<num_nn;j++) {
				
				if (nn_match[j]>max_matched){
					max_matched = nn_match[j];
					
					d->mad     = 0.9;
					d->grain   = nn[j]->grain;
					d->orientation = nn[j]->orientation;
					
					assigned++;
					
				}
			}
			
		}//endif: check mad
	}//endfor: i
	
	return 1;
};