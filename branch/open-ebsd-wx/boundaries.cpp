/**	\file 	boundaries.cpp
	\brief	Functions for visualising the entire boundary network	
*/

#include "ebsd.h"

#define QUAD_BUFFER 4000	//!< How many Quad to allocate memory for at a time

/// Create a colour lookup table for the boundary network by grain
vtkLookupTable* Dataset::boundary_lut() 
{
	
	int i;
	float *e;
	
	vtkLookupTable* lut = vtkLookupTable::New(); 
	lut->SetNumberOfColors(num_grains);
	
	for(i=0;i<num_grains;i++){
		e = grains[i].orientation.euler();
		lut->SetTableValue(i,e[0]/255,e[1]/255,e[2]/255,0.5);
	}
	
	return lut;
	
}

/// Draw the entire boundary network, mapping disorientations
vtkPolyData* Dataset::boundary_network()
{
	int index, i,j,k = 0, exists;
	int x,y,z;
	int num_quads = 0, new_quads = 0, buffer_count = 1;
	int num_vertices = 0;
	int num_adjacent;
	
	AdjGrain* adj_grain;
	Quad *quads;
	XYZ p;
	Point *d = data;
	
	// Allocate buffer for quads (faces)
	// We will expand this as required
	quads = (Quad*) malloc(sizeof(Quad)*QUAD_BUFFER);
	if (quads == NULL) { return 0; }


	// Cycle through each 					
	//--------------------------------------------------
	for(i=0;i<nv;i++){
		d++;	
		
		if (d->grain>=0) {

			// Calcuate the x,y,z coordinates from the voxel index					
			//--------------------------------------------------		
			p.x = (i/dx) % tx;
			p.y = (i/dy) % ty;
			p.z = (i/dz) % tz;
	
			// Check if we need to increase the size of the quad buffer					
			//--------------------------------------------------
			if(num_quads+24 > buffer_count*QUAD_BUFFER) {
				quads = (Quad*) realloc(quads, sizeof(Quad)*(++buffer_count)*QUAD_BUFFER);
				if (quads == NULL) { return 0; }
			}
			
			// Check adjacent voxels in the positive x,y,z directions 
			// and add a quad if they're not in the same grain
			//--------------------------------------------------
			if(p.x<tx-1 && (d+dx)->grain>=0 && (d+dx)->grain != d->grain){
				make_patch(p,X,+1,quads+num_quads, steps,(d+dx)->grain,d->grain);
				num_quads++;
			}
	
			if(p.y<ty-1 && (d+dy)->grain>=0 && (d+dy)->grain != d->grain){
				make_patch(p,Y,+1,quads+num_quads, steps,(d+dy)->grain,d->grain);
				num_quads++;
			}
			
			if(p.z<tz-1 && (d+dz)->grain>=0 && (d+dz)->grain != d->grain){
				make_patch(p,Z,+1,quads+num_quads, steps,(d+dz)->grain,d->grain);
				num_quads++;
			}		
			
		}	
	}

	printf("Number of quads: %i\n",num_quads);

	// Create a symmetric matrix of disorientations between all (adjacent) grains 
	// for fast access - we'll be doing lots of these lookups. 
	//--------------------------------------------------
	float *dis = NULL;
	dis = (float*) malloc(num_grains*num_grains*sizeof(float));
	if (dis == NULL) { return 0; }
	
	// initialise the disorientations to invalid value for debugging
	// this can be safely removed in implementation
	for(i=0;i<num_grains;i++){
	for(j=0;j<num_grains;j++){
		dis[j+i*num_grains] = 100;
	}}
	
	for(i=0;i<num_grains;i++){
		for(j=0;j<grains[i].num_adj_grains;j++){
			
			adj_grain = grains[i].adj_grains+j;
			dis[i+num_grains*adj_grain->grain] = adj_grain->disorientation;
			dis[adj_grain->grain+num_grains*i] = adj_grain->disorientation;
						
		}
	}
	//--------------------------------------------------
	
	
	// Set up the VTK objects
	//--------------------------------------------------
	vtkPoints* pts=vtkPoints::New();
	pts->SetNumberOfPoints(num_quads*4);
	
	vtkCellArray* gquads = vtkCellArray::New();
	int size = gquads->EstimateSize(num_quads,4);
	gquads->Allocate(10, 5);

	// ptr is direct pointer to underlying gquads representation,
	// see VTK documentation for details
	int *ptr = (int*) gquads->WritePointer(num_quads,size);
	
	// colors is VTK array storing the disorientation value for each face
	vtkFloatArray* colours = vtkFloatArray::New();
	colours->SetName("disorientations");
	colours->SetNumberOfComponents(1);
	colours->SetNumberOfTuples(num_quads);

	// Fill the VTK objects
	//--------------------------------------------------
	for(i=0;i<num_quads;i++){
		colours->InsertValue( i, dis[quads[i].grain1+quads[i].grain2*num_grains] );

		*(ptr++) = 4;		
		for(j=0;j<4;j++){
			*(ptr++) = k;
			pts->SetPoint(k, (double)quads[i].p[j].x,
			   				 (double)quads[i].p[j].y,
							 (double)quads[i].p[j].z);
			k++;
		}	
	}
			
	vtkPolyData* surface = vtkPolyData::New();
	surface->SetPoints(pts);
	surface->SetPolys(gquads);
	surface->GetCellData()->SetScalars(colours);
	
	
	// Clean up
	//--------------------------------------------------
	free(quads);	
	pts->Delete();
	gquads->Delete();
	free(dis);
	
	return surface;

}
