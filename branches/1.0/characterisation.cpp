/**	\file 	characterisation.cpp
	\brief	Functions for characterising the grains
	
	Contains functions to calculate grain averages, surfaces and equivilent ellipsoids
*/

#include "ebsd.h"

/// Convenience function for reconstructing grains and calcuating averages, adjacencies
int Dataset::calculate_grain_info(){
	reconstruct_grains();
	average_orientations();	
	adjacency_list();
	return 1;
}

/// Convenience function for calculating statistics not required by 
int Dataset::calculate_extra_stats(){
	calculate_ellipses();
    calculate_surface();
	return 1;
}

/// Calculate average orientation of each grain
int Dataset::average_orientations()
{
	int i;
		
	//#pragma omp parallel for
	for(i=0;i<num_grains;i++){
		
		int	k;
		Point *d0, *d1, *d;
		int volume, valid_count;
		long double avg[4];
		long double q[4], n;
		
		if (i%100 == 0) {  
			//#pragma omp critial(display)
			progress_callback((float)i / num_grains,"Calculating grain averages", false);
		}
		
		volume = grains[i].volume;
		
		for(k=0;k<4;k++){	
			avg[k] = 0;
		}
		
		d0 = data + grains[i].list_head->index;	 	
	 	d0->orientation.reduce_zone();
	 	
		class GrainList *list, *list_next;
		
		d = data + grains[i].list_head->index;
		int qq = 0;
		valid_count = 0;
		
		// Cycle through each voxel in grain and calculate average
		//--------------------------------------------------
		for(list=grains[i].list_head; list!=NULL && valid_count < 100; list=list->next){
			list_next = list->next;
			
			d0 = data + list->index;

			if(d0->orientation.is_non_zero()){
				for(k=0;k<4;k++){
					avg[k] += (long double)d0->orientation.q[k];
					assert(!isnan(avg[k]));
				}
				valid_count++;
			}
			qq++;
			
			// Place each voxel in the same cloud as the previous
			//--------------------------------------------------
			if (list->next!=NULL) {
				d1 = data + list_next->index;				
				d1->orientation.reduce_zone(&(d->orientation));
			}
			
		}
		
		if (valid_count == 0){
			grains[i].orientation.set_zero();
		} else {
		
			// Renormalise average value and reduce to fundamental zone
			//--------------------------------------------------
			n = 0;
			for(k=0;k<4;k++){	
				q[k] = avg[k]/valid_count;
				assert(!isnan(q[k]));
				n = n + q[k]*q[k]; 
			}
			n = sqrt(n);
			
			for(k=0;k<4;k++){	
				q[k] = q[k]/n;
			}		
	
			grains[i].orientation = q;
			grains[i].orientation.reduce_zone();
			
		}
	}
	
	progress_callback(1,"Calculating grain averages", false);

	return 1;
}

// Used for quicksort of axes in calcuate_ellipsoids
// Return value = | -1 if a > b
//				  |  0    a = b  
//				  | +1    a < b
int compare_floats(const void *a, const void *b)
{
	float t= *(float*)a-*(float*)b;
	if (t<0) {
		return 1;
	} else if (t>0) {
		return -1;
	} else {
		return 0;
	}
}

/// Calculate grain surfaces and surface areas
/** 
	Uses modified marching cubes algorithm
*/
int Dataset::calculate_surface()
{
	Gridcell grid;
	int index, in,
		i,j,k;
	
	int num_triangles = 0, 
		new_triangles = 0, 
		*buffer_count;
		
	buffer_count = (int*) malloc(sizeof(int)*num_grains);
	if (buffer_count == NULL) { return 0; }	
	
	double sa = 0,
		   AB[3], AC[3], ABAC;
	
	Triangle *t;
			 
	bool already_marked;



	// For each grain allocate initial buffer and set surface count to zero
	//--------------------------------------------------
	for(i=0;i<num_grains;i++){

		if (grains[i].surface_triangles != NULL) {
			free(grains[i].surface_triangles);
		}
					
		grains[i].surface_triangles = (Triangle*) malloc(sizeof(Triangle)*TRIANGLE_BUFFER);
		if (grains[i].surface_triangles == NULL) { return 0; }
					
		grains[i].num_triangles = 0;
		buffer_count[i] = 1;
	}
	
	XYZ pi;
	
	int present[9];
	int num_present;

	present[0] = -1;

	// Cycle through each 'cube' in dataset and calculate the surface for 
	// each of the grains present in the cube. 
	//--------------------------------------------------
	for(index=0;index<nv;index++){
		
		if (index%1000 == 0) {  
			//#pragma omp critial(display)
			progress_callback((float)index / nv,"Calculating surfaces", false);
		}
		
		pi.x = (index % tx)-1;
		pi.y = (index/dy % ty)-1;
		pi.z = (index/dz % tz)-1;
		
		num_present = 1;
		
		for(j=0;j<8;j++) {
			grid.p[j].x = pi.x + grid_offset[j][X];
			grid.p[j].y = pi.y + grid_offset[j][Y];
			grid.p[j].z = pi.z + grid_offset[j][Z];
			
			if (grid.p[j].x<0       || grid.p[j].y<0       || grid.p[j].z<0 ||
				grid.p[j].x>=(tx-2) || grid.p[j].y>=(ty-2) || grid.p[j].z>=(tz-2)) { 
				grid.grain[j] = -1;
			} else {
				in = index - dz + grid_offset[j][X] + grid_offset[j][Y]*dy + grid_offset[j][Z]*dz;
				grid.grain[j] = data[in].grain;

				// Keep track of which grains are present
				//--------------------------------------------------
				if (data[in].grain >= 0) {
					already_marked = false;
					for(k=0;k<num_present;k++){
						if (data[in].grain == present[k]) {
							already_marked = true;
						}
					}
					if (!already_marked) {
						present[ num_present++ ] = data[in].grain;
					}
				}
			}
		}
		
		
		for(j=1;j<num_present;j++){
			num_triangles = grains[ present[j] ].num_triangles;
			
			// Make sure there's enough space in the buffer
			//--------------------------------------------------
			if(num_triangles+12 > buffer_count[ present[j] ]*TRIANGLE_BUFFER) {
				grains[ present[j] ].surface_triangles = (Triangle*) realloc(grains[ present[j] ].surface_triangles, sizeof(Triangle)*(++buffer_count[present[j]])*TRIANGLE_BUFFER);
				if (grains[ present[j] ].surface_triangles == NULL) { return 0; }
			}	
			
			new_triangles = polygonise(grid, present[j], grains[ present[j] ].surface_triangles+num_triangles);
			grains[ present[j] ].num_triangles += new_triangles;
		}
	}

	for(i=0;i<num_grains;i++){			
		
		sa = 0;	
		
		t = grains[i].surface_triangles;
		
		// Calculate surface area of grains
		//--------------------------------------------------
		for(j=0;j<grains[i].num_triangles;j++){
					
			AB[X] = (t->p[0].x - t->p[1].x)*(double)steps[X];
			AB[Y] = (t->p[0].y - t->p[1].y)*(double)steps[Y];
			AB[Z] = (t->p[0].z - t->p[1].z)*(double)steps[Z];
			
			AC[X] = (t->p[0].x - t->p[2].x)*(double)steps[X];
			AC[Y] = (t->p[0].y - t->p[2].y)*(double)steps[Y];
			AC[Z] = (t->p[0].z - t->p[2].z)*(double)steps[Z];
			
			ABAC = AB[X]*AC[X]+AB[Y]*AC[Y]+AB[Z]*AC[Z];
			
			sa += 0.5 * sqrt( (AB[X]*AB[X] + AB[Y]*AB[Y] + AB[Z]*AB[Z])*
							  (AC[X]*AC[X] + AC[Y]*AC[Y] + AC[Z]*AC[Z])-
							  ABAC*ABAC ); 

			t++;
								
		}
	
		grains[i].surface_area = (float) sa;
	}
	
	free(buffer_count);
	
	progress_callback(1,"Calculating grain surfaces", false);
}
 

/// Calcuate ellipsoids with same moments of inertia, centroids as grains 
int Dataset::calculate_ellipses()
{
	int i;
	int done = 0;
	
	//#pragma omp parallel for
	for(i=0;i<num_grains;i++){	

		int j,k,
			index;

		SymmetricMatrix I2(3); I2 = 0;
		DiagonalMatrix D(3);
		
		double I1[3], A[3], I0, p[3], c[3], ax[3], I2x[3];
		class GrainList *list;
	
		progress_callback((float)done/num_grains, "Calculating ellipses", false);
		
		I0 = grains[i].volume;
		
		I1[0] = 0; 	I1[1] = 0;	I1[2] = 0;
	
		if(grains[i].volume > 1){
		
			// Calculate 1st moments of inertia
			//--------------------------------------------------		
			for(list=grains[i].list_head; list!=NULL; list=list->next){
				index = list->index;
				p[X] = (index % tx)     *(double)steps[X];
				p[Y] = ((index/dy) % ty)*(double)steps[Y];
				p[Z] = ((index/dz) % tz)*(double)steps[Z];

				for(j=0;j<3;j++){
					I1[j] += p[j];
				}					
			}
			
			// Calculate centroid
			//--------------------------------------------------		
			for(j=0;j<3;j++){
				c[j] = I1[j]/I0;
			}
			
			// Calculate 1st, 2nd moments of inertia
			//--------------------------------------------------		
			for(list=grains[i].list_head; list!=NULL; list=list->next){
				index = list->index;
				p[X] = (index % tx)     *(double)steps[X] - c[X];
				p[Y] = ((index/dy) % ty)*(double)steps[Y] - c[Y];
				p[Z] = ((index/dz) % tz)*(double)steps[Z] - c[Z];
				
				I2(1,1) += p[1]*p[1] + p[2]*p[2];		// Newmat library uses
				I2(2,2) += p[0]*p[0] + p[2]*p[2];		// 1-indexed arrays
				I2(3,3) += p[0]*p[0] + p[1]*p[1];
				
				I2(1,2) -= p[0]*p[1];
				I2(1,3) -= p[0]*p[2];
				I2(2,3) -= p[1]*p[2];		
			}
			
			for(j=1;j<=3;j++){
				I2(j,j) = I2(j,j)/I0;
				for(k=j+1;k<=3;k++){
					I2(j,k) = I2(j,k)/I0;
				}
			}
		
			// Diagonalise matrix
			Jacobi(I2,D);
		
			// Calcuate axes
			//--------------------------------------------------					
			for(j=0;j<3;j++){
				A[j] = 15/(8*PI)*( D(j+1,j+1) + D((j+1)%3+1,(j+1)%3+1) - D((j+2)%3+1,(j+2)%3+1) );
			}
		
			I2x[0] = D(1,1);
			I2x[1] = D(2,2);
			I2x[2] = D(3,3);
		
			for(j=0;j<3;j++){
				ax[j] = sqrt(2.5*(I2x[(j+1)%3]+I2x[(j+2)%3]-I2x[j]));
			}
			
			for(j=0;j<3;j++){
				grains[i].centroid[j] = c[j];	
				grains[i].axis[j] = ax[j]; //pow(pow(A[j],4)/(A[(j+1)%3]*A[(j+2)%3]),0.1);
			}

			// Sort axes such that a>b>c
			qsort(grains[i].axis,3,sizeof(float),compare_floats);
			
			if (isnan(grains[i].axis[0]) || isnan(grains[i].axis[1]) || isnan(grains[i].axis[2]) ) { //||
				//isinf(grains[i].axis[0]) || isinf(grains[i].axis[1]) || isinf(grains[i].axis[2])) {
				grains[i].axis[0] = 1;
				grains[i].axis[1] = 1;
				grains[i].axis[2] = 1;
			}			
		}
		
		#pragma omp atomic
		done++;
		
	}
		
	progress_callback(1, "Calculating ellipses", false);
		
	return 1;
}


/// Determine the triangless making up the surface in a given grid using the Marching Cubes algorithm
int polygonise(Gridcell grid, int grain, Triangle *triangles)
{
	int i, ntriang;
	int cubeindex;
	XYZ vertlist[12];
	int grainlist[12];

	/*
		Determine the index into the edge table which
		tells us which vertices are inside of the surface
	*/
	cubeindex = 0;
	if (grid.grain[0] != grain) cubeindex |= 1;
	if (grid.grain[1] != grain) cubeindex |= 2;
	if (grid.grain[2] != grain) cubeindex |= 4;
	if (grid.grain[3] != grain) cubeindex |= 8;
	if (grid.grain[4] != grain) cubeindex |= 16;
	if (grid.grain[5] != grain) cubeindex |= 32;
	if (grid.grain[6] != grain) cubeindex |= 64;
	if (grid.grain[7] != grain) cubeindex |= 128;

	/* Cube is entirely in/out of the surface */
	if (edgeTable[cubeindex] == 0)
		return(0);

	/* Find the vertices where the surface intersects the cube */
	if (edgeTable[cubeindex] & 1) {
		vertlist[0]	= VertexInterp(grid.p[0],grid.p[1]);
	}
	if (edgeTable[cubeindex] & 2) {
		vertlist[1]	= VertexInterp(grid.p[1],grid.p[2]);
	}
	if (edgeTable[cubeindex] & 4) {
		vertlist[2]	= VertexInterp(grid.p[2],grid.p[3]);
	}
	if (edgeTable[cubeindex] & 8) {
		vertlist[3]	= VertexInterp(grid.p[3],grid.p[0]);
	}
	if (edgeTable[cubeindex] & 16) {
		vertlist[4]	= VertexInterp(grid.p[4],grid.p[5]);
	}
	if (edgeTable[cubeindex] & 32) {
		vertlist[5]	= VertexInterp(grid.p[5],grid.p[6]);
	}
	if (edgeTable[cubeindex] & 64) {
		vertlist[6]	= VertexInterp(grid.p[6],grid.p[7]);
	}
	if (edgeTable[cubeindex] & 128) {
		vertlist[7]	= VertexInterp(grid.p[7],grid.p[4]);
	}
	if (edgeTable[cubeindex] & 256)	{
		vertlist[8]	= VertexInterp(grid.p[0],grid.p[4]);
	}
	if (edgeTable[cubeindex] & 512) { 
		vertlist[9]	= VertexInterp(grid.p[1],grid.p[5]);
	}
	if (edgeTable[cubeindex] & 1024) {
		vertlist[10] = VertexInterp(grid.p[2],grid.p[6]);
	}
	if (edgeTable[cubeindex] & 2048) {
		vertlist[11] = VertexInterp(grid.p[3],grid.p[7]);
	}
	/* Create the triangle */
	ntriang = 0;
	for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
		triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i  ]];
		triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i+1]];
		triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i+2]];
		ntriang++;
	}

	return(ntriang);
}

/// Calculate the midpoint of two positions
XYZ VertexInterp(XYZ p1, XYZ p2)
{
	XYZ p;

	p.x =  (p1.x + p2.x)*0.5;
	p.y =  (p1.y + p2.y)*0.5;
	p.z =  (p1.z + p2.z)*0.5;

	return(p);
}
