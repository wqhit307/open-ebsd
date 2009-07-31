/**
	\file segmentation.cpp
	\brief Functions for segmenting the dataset into grains
*/

#include "ebsd.h"

#define push(x)			stack[stack_top++] = x
#define pop()			stack[--stack_top]

#define gl_add(i)		grain_list_free->index = i; \
						grain_list_free->next = grain_list_free+1; \
						grain_list_free++
	
#define CRIT_DIFF		14

/// Segment dataset into contiguous grains with neighbouring misorientations below a critial threshold 
/**
	\param min_angle Angular misorientatation threshold used to segment grains
	
	Uses a region expansion algorithm; starts with one point and look at all nearest 
	neighbours. Add points with misorientation below threshold threshold. Repeats for all
	newly added points until no more are added. Iterates over all remaining points that 
	have not been allocated
*/
int Dataset::segment(float min_angle)
{
	Point *d, *nn,
		  **stack;
	
	int i, j;
	int assigned_count = 0,
		cur_grain      = 0,
		stack_top	   = 0;

	int nn_offset_edge[NUM_NN+1];
	int *nn_offset, num_nn;

	// Delete old grains if allocated					
	//--------------------------------------------------
	if (grains != NULL) { 
		for(i=0;i<num_grains;i++){
			if( grains[i].adj_grains != NULL && grains[i].num_adj_grains > 0 ){
				free(grains[i].adj_grains);
			}
			if( grains[i].num_triangles > 0 ){
				free(grains[i].surface_triangles);
			}
		}
		
		free(grains);
		grains = NULL;
	}
	
	// Precalculate tolerance for segmentation
	//--------------------------------------------------
	float quat_tol = cos(min_angle*0.5*DEG);	
	
	// Create and allocate stack. Allocates enough to 
	// ensure no range checking is needed
	//--------------------------------------------------
	stack = (Point**) malloc( nv * sizeof(int) );
	if (stack == NULL) { return 0; }
	
	// Initialise all points to grain '-1'; indexing
	// starts from zero 					
	//--------------------------------------------------	
	for(i=0;i<nv;i++) {
		data[i].grain = -1;
	}
	
	for(i=0;i<nv;i++) {
		d = data + i;
		
		// Check point is valid, and has not already be allocated
		if (d->grain==-1 && d->mad > MAD_THRESHOLD && !d->external) {
			
			// Start a new grain and push to stack
			d->grain = cur_grain;
			push(d);

			// Display progress
			if (assigned_count%100 == 0) {  
				progress_callback((float)assigned_count/nv,"Segmenting grains", false);
			}

			allow_py_threading;
		
			// Begin region expansion; start with one point and
			// look at all nearest neighbours. Add points with 
			// misorientation below threshold threshold and then
			// look their nearest neighbours
			//--------------------------------------------------
			while (stack_top > 0) {
				d = pop();
				//Check point is valid
				if (d->mad > MAD_THRESHOLD && !d->external) {
					
					// Get list of offsets to nearest neighbours
					nearest_neigbours(d-data,&nn_offset,nn_offset_edge,&num_nn);
					
					// Cycle through nearest neighbours for valid points
					// with misorientation within threshold
					//--------------------------------------------------
					for(j=1;j<num_nn;j++){
						nn = d + nn_offset[j];
						assert(nn-data<nv);
						if ( nn->grain == -1 && !nn->external ) {
							if (d->orientation.disorientation_threshold(&(nn->orientation), quat_tol)) {
//							if(disorientation_threshold(nn->orientation.q,d->orientation.q,quat_tol)){	
								// If same grain, allocate point and push to stack
								//--------------------------------------------------				
								assigned_count++;
								nn->grain = cur_grain;
								//add to stack
								push(nn);
								
							}
						} 
					} 
				}
			}
			
			cur_grain++;
			
			block_py_threading;
			
		} //endif grain
	} //endfor all
	
	//Set number of grains	
	num_grains = cur_grain;
	printf("Grains: %i\n",num_grains);
				
	free(stack);

	// Calculate grain statistics
	calculate_grain_info();

	return 1;
}

/// Construct grain objects from a dataset whose points have already been allocated grains
int Dataset::reconstruct_grains()
{
	Point *d, *nn;
	int i, j,
		stack_top = 0, 
		cur_grain = 0,
		g_top = 0;
	int nn_index, index, num_nn;
		
	int *stack = NULL,
		*grain_num = NULL, 
		*grain_vol;

	bool *grain_on_edge;

	GrainList **grain_ptrs = NULL;

	int nn_offset_edge[NUM_NN+1];
	int *nn_offset;
	
	// Check if then dataset has been segmented 					
	//--------------------------------------------------
	if (num_grains == 0) {
		return 0;
	}
	
	// Free data allocated for old grains 					
	//--------------------------------------------------
	if (grains != NULL) { 
		for(i=0;i<num_grains;i++){
			if( grains[i].adj_grains != NULL && grains[i].num_adj_grains > 0 ){
				free(grains[i].adj_grains);
			}					
			if( grains[i].num_triangles > 0 ){
				free(grains[i].surface_triangles);
			}

		}
		
		free(grains);
		grains = NULL;
	}
	
	// Allocate variables and initialise grain numbers to -1					
	//--------------------------------------------------
	grain_list_free = grain_list_head;
	
	stack 		  = (int*) malloc( nv * sizeof(int) );
	grain_num     = (int*) malloc( nv * sizeof(int) );
	grain_ptrs    = (GrainList**) malloc( num_grains * sizeof(int) );
	grain_vol     = (int*) malloc( num_grains * sizeof(int) );
	grain_on_edge = (bool*) malloc( num_grains * sizeof(bool) );
	
	if (stack == NULL || grain_ptrs == NULL ||
		grain_num == NULL || grain_vol == NULL || grain_on_edge == NULL ) { return 0; }

	for(i=0;i<nv;i++) {
		grain_num[i] = -1;
	}
	//--------------------------------------------------

	allow_py_threading;

	for(i=0;i<nv;i++) {
		d = data + i;
		 					
		// Check if grain is valid and hasn't been allocated					
		//--------------------------------------------------
		if (grain_num[i]==-1 && d->mad > 0 && !d->external) {

			grain_num[i] = cur_grain;
			push(i);

			grain_vol[cur_grain] = 1;
			grain_ptrs[cur_grain] = grain_list_free;
			grain_on_edge[cur_grain] = false;
			gl_add(i);

			while (stack_top > 0) {
				index = pop();
				d = data + index;
				
				nearest_neigbours(index,&nn_offset,nn_offset_edge,&num_nn);
				
				//cycle nearest neighbours
				for(j=1;j<num_nn;j++){
					nn_index = index + nn_offset[j];
					nn = data + nn_index;

					if(nn->external) {
						grain_on_edge[cur_grain] = true;
					}

					//check if grain is not assigned and same as current
					if ( grain_num[nn_index]==-1 && !nn->external ) {
						if ( nn->grain == d->grain ) {
							//set grain 
							grain_num[nn_index] = cur_grain;
							grain_vol[cur_grain]++;
							gl_add(nn_index);

							//add to stack
							push(nn_index);
						}
					} 	
				} 
			}
			
			cur_grain++;
			
		} //endif grain
	} //endfor all
		
	for(i=0;i<nv;i++){
		d = data + i;
		d->grain = grain_num[i];
	}
	
	num_grains = cur_grain;
	printf("Grains: %i\n",cur_grain);
	
	
	// Allocate and populate grains					
	//--------------------------------------------------	
	grains = (Grain*) malloc( sizeof(Grain)*num_grains );
	if (grains == NULL) { return 0; }
		
	for(i=0;i<num_grains;i++){
		grains[i].volume = grain_vol[i];
		grains[i].adj_grains = NULL;
		grains[i].surface_triangles = NULL;
		grains[i].on_edge = grain_on_edge[i];

		// Set list head and tail 					
		//--------------------------------------------------
		grains[i].list_head = grain_ptrs[i];
		grains[i].list_tail = grain_ptrs[i]+grain_vol[i]-1;	
		grains[i].list_tail->next = NULL;
		
	}

	block_py_threading
		
	// Free memory 					
	//--------------------------------------------------	
	free(grain_num);
	free(stack);
	free(grain_ptrs);
	free(grain_vol);
	free(grain_on_edge);

}

/// For each grain create list of all adjacent grains 
int Dataset::adjacency_list()
{
	int grain;
	int pc = num_grains / 50 + 1;

	Point *d, *nn;
	int i, j, k, l, 
		diff_neighbours;
	
	int nn_offset_edge[NUM_NN+1],
		*nn_offset, num_nn;

	AdjGrain *adj;

	GrainList *list;
	Grain *g;

	int *adj_grain_count, num_adj_grains;
	adj_grain_count = (int*) malloc(num_grains*sizeof(int));
	if (adj_grain_count == NULL) { return 0; }

	
	for(i=0;i<num_grains;i++){
		g = grains+i;
		if (g->volume > 0) {
			
			//Create adjacency lists
			
			g->surface_points = 0;
			for(j=0;j<num_grains;j++){
				adj_grain_count[j] = 0;
			}
			
			j=0;
			for(list = g->list_head; list!=NULL; list=list->next){	
				j++;
				d = data + list->index;
				diff_neighbours = 0;
				nearest_neigbours(list->index,&nn_offset,nn_offset_edge,&num_nn);
				//cycle nearest neighbours
				for(l=1;l<num_nn;l++){
					nn = d + nn_offset[l];
		
					if ( !nn->external && nn->grain!=-1 && nn->grain != d->grain ) {
						diff_neighbours++;
									
						adj_grain_count[ nn->grain ]++;					
					} //endif check grain
				} //endfor cycle nn
				
				if (diff_neighbours > CRIT_DIFF) {
					g->surface_points++;
				}	
			}
			
			num_adj_grains = 0;
			for(j=0;j<num_grains;j++){
				if (adj_grain_count[j] > 0){
					num_adj_grains++;
				}
			}

			if (num_adj_grains > 0) {

				if (g->adj_grains == NULL) {
					g->adj_grains = (AdjGrain*) malloc( num_adj_grains * sizeof(AdjGrain) );
				} else if (g->num_adj_grains < num_adj_grains) {
					g->adj_grains = (AdjGrain*) realloc( g->adj_grains, num_adj_grains * sizeof(AdjGrain) );
				}
		
				g->num_adj_grains = num_adj_grains;
							
				//calculate disorientations with adjacent grains and populate adj grain information
				k = 0;
				for(j=0;j<num_grains;j++){
					if (adj_grain_count[j] > 0){
						adj = g->adj_grains + k++;
						
						adj->grain = j;
						adj->disorientation = g->orientation.disorientation(&(grains[j].orientation));
						adj->nn_matches = adj_grain_count[j];
					}
				}
				assert(g->adj_grains >0); 
			
			} else {
				g->num_adj_grains = 0;
			}
		}
		if (i%pc == 0){
			progress_callback((float)(i+1)/num_grains,"Calculating adjacency list", false);
		}
	}
	
	progress_callback(1.0,"Calculating adjacency list", false);
	free(adj_grain_count);
	
}

/// Return offsets to all nearest neighbours of a point which lie in dataset
/**
	@param index index of the point
	@param nn pointer to return list of nearest neighbours (could point to nn_edge or nn_bulk)
	@param nn_edge space to store values if point is on edge of dataset
	@param num_nn return values, number of nearest neighbours
	@param mode 0 for all neighbours, 1 for nn in adjacent slices (for interpolation)
*/
void Dataset::nearest_neigbours(int index, int **nn, int *nn_edge, int *num_nn, int mode) 
{
	int i,j,k;
	int p[3],
		s_d[3],
	 	start = 0,
	 	n_d   = 3;

	bool edge = false;

//	s_d = [] 0 -> both, Ê1 -> + only, -1 -> -only

	// Calculate position of point in x,y,z	
	//--------------------------------------------------	
	p[X] = index % tx;
	p[Y] = (index/dy) % ty;
	p[Z] = (index/dz) % tz;
	
	// Determine whether point is on edge, and which 
	// directions we can extend in:
	//		s_d[x] = | -1  if only in negative x dir
	//				 | +1  if only in positive x dir
	//				 |  0  if both 
	//--------------------------------------------------	
	for(i=0;i<3;i++) {
		if (p[i] == 0) {
			s_d[i] = 1;
			edge = true;
		} else if (p[i] == dims[i]-1) {
			s_d[i] = -1;
			edge = true;
		} else {
			s_d[i] = 0;
		}
	}

	if (!edge && mode == 0) {

		// If we're in the bulk use precalculated offsets
		//--------------------------------------------------		
		*nn = nn_offset_bulk;
		*num_nn = NUM_NN+1;
		
	} else {

		// If we're on the edge use the buffer provided
		//--------------------------------------------------		
		*nn = nn_edge;
		*num_nn = 1;

//		for(j=0;j<=NUM_NN;j++){
//			*(*nn+j) = 0;
//		}

		(*nn)[0] = 0;

		// Loop through each dimension and add ±d[x] to each
		// of the offsets already there (seed with zero)
		// Algorithm with progress:
		//		0
		//		0,+z,-z
		//		0,+z,-z,+y,-y,+z+y,+z-y,-z+y,-z-y
		//		...
		// Leaving out anything that extends out of the
		// dataset as determined before
		//--------------------------------------------------		
		for(n_d=Z;n_d>=X;n_d--) {
			k=0;	
			for(j=start;j<*num_nn;j++) { 
				if (s_d[n_d] > -1)
					(*nn)[*num_nn+(k++)] = (*nn)[j] + d[n_d];
				if (s_d[n_d] < 1)
					(*nn)[*num_nn+(k++)] = (*nn)[j] - d[n_d];
			}
			if (mode == 1 && n_d==Z) {
				// If we don't want to choose points in the same plane short
				// circuit initial zero seed once we've got ±z
				start = 1;	
			}
			*num_nn=*num_nn+k;
		}
	}
	
}
