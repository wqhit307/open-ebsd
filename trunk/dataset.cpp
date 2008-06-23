/**
	\file dataset.cpp
	\brief Functions for creating and initialising dataset	
*/

#include "ebsd.h"

/// Create dataset from raw file 
/**
	\param file Location of a raw file exported from this software
*/
Dataset::Dataset(const char file[], Callback *c)
{
	init_params(c);
	init = (bool)read_raw(file);
	if (init) {
		init_geometry();
		if (num_grains > 0) {
			calculate_grain_info();
		}
	}
	
}

/// Create dataset from Channel 5 project
/**
	\param file Location of a Channel 5 CSV export
	\param dimensions Extent of the data in form [x,y,z]
	\param data_steps Spacing between data points in [x,y,z] directions
*/
Dataset::Dataset(const char file[], int dimensions[], float data_steps[], Callback *c)
{
	init_params(c);
	init_geometry(dimensions,data_steps);

	init = (bool)read_channel5(file);
}

void Dataset::init_params(Callback *c)
{
	grain_list_head = NULL;
	grains = NULL;
	num_grains = 0;
	
	offsets_calculated = false;
	marked 	    = NULL;
	suspicious  = NULL;
	offset_x    = NULL;
	offset_y    = NULL;
	probability = NULL;
	vtk_dataset = NULL;

	callback    = c;

	init = false;	
}

/// Set dimensions and step legths of data, initialise commonly used geometry parameters
/**
	\param dimensions Extent of the data in form [x,y,z]
	\param data_steps Spacing between data points in [x,y,z] directions
*/
void Dataset::init_geometry(int *dimensions, float *data_steps)
{
	int i;
	
	if (dimensions!=NULL) {
		memcpy(dims, dimensions, 3*sizeof(int));
	}
	if (data_steps!=NULL) {
		memcpy(steps, data_steps, 3*sizeof(float));
	}

	//Shorthand for dimensions	
	tx = dims[X];
	ty = dims[Y];
	tz = dims[Z];
	
	//Map movement in x,y,z direction onto displacements in 1D array
	dx = 1;
	dy = tx;
	dz = tx*ty;

	d[X] = dx;
	d[Y] = dy;
	d[Z] = dz;
	
	//Total number of points
	nv = dz*tz;
	
	//Offsets to nearest neighbours for points in the bulk (i.e. not on the edge)
	nn_offset_bulk[ 0] =  0;
	nn_offset_bulk[ 1] =  dx;
	nn_offset_bulk[ 2] = -dx;
	nn_offset_bulk[ 3] =  dy;
	nn_offset_bulk[ 4] = -dy;
	nn_offset_bulk[ 5] =  dz;
	nn_offset_bulk[ 6] = -dz;
	nn_offset_bulk[ 7] = +dx+dy;
	nn_offset_bulk[ 8] = +dx-dy;
	nn_offset_bulk[ 9] = -dx+dy;
	nn_offset_bulk[10] = -dx-dy;
	nn_offset_bulk[11] = +dy+dz;
	nn_offset_bulk[12] = +dy-dz;
	nn_offset_bulk[13] = -dy+dz;
	nn_offset_bulk[14] = -dy-dz;
	nn_offset_bulk[15] = +dz+dx;
	nn_offset_bulk[16] = +dz-dx;
	nn_offset_bulk[17] = -dz+dx;
	nn_offset_bulk[18] = -dz-dx;
	nn_offset_bulk[19] = +dx+dy+dz;
	nn_offset_bulk[20] = +dx+dy-dz;
	nn_offset_bulk[21] = +dx-dy+dz;
	nn_offset_bulk[22] = +dx-dy-dz;
	nn_offset_bulk[23] = -dx+dy+dz;
	nn_offset_bulk[24] = -dx+dy-dz;
	nn_offset_bulk[25] = -dx-dy+dz;
	nn_offset_bulk[26] = -dx-dy-dz;

	if (grain_list_head == NULL) {
		grain_list_head = (class GrainList*) malloc( nv * sizeof(GrainList) );
	} else {
		grain_list_head = (class GrainList*) realloc( grain_list_head, nv * sizeof(GrainList) );
	}
	grain_list_free = grain_list_head;
	
	if (marked == NULL) {
		marked 	   = (bool*) malloc(tz*sizeof(bool));
		suspicious = (bool*) malloc(tz*sizeof(bool));
		offset_x   = (int*)  malloc(tz*sizeof(int));
		offset_y   = (int*)  malloc(tz*sizeof(int));
		probability= (float*)malloc(tz*sizeof(float));
		
		for(i=0;i<tz;i++){
			marked[i] = false;
			suspicious[i] = false;
		}
		
	} else {
		marked 	   = (bool*) realloc(marked, tz*sizeof(bool));
		suspicious = (bool*) realloc(suspicious,tz*sizeof(bool));
		offset_x   = (int*)  realloc(offset_x,tz*sizeof(int));
		offset_y   = (int*)  realloc(offset_y,tz*sizeof(int));
		probability= (float*)realloc(probability,tz*sizeof(float));
	}


}

