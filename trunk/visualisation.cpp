/**
	\file visualisation.cpp
	\brief Functions for visualising the dataset
*/

#include "ebsd.h"

/// Produce a VTK StructuredPoints class for the dataset, will be used to generate orthogonal slices
/**
	@param mode	specifies how to display the data. One of MODE enum.
*/
vtkObject* Dataset::vtk_3d_dataset(int mode){

	float *e;
	int i;	
		
	if (vtk_dataset!=NULL) {
		vtk_dataset->Delete();
	}
	
	vtk_dataset = vtkStructuredPoints::New();
	vtk_dataset = vtk_dataset;
	vtk_dataset->SetNumberOfScalarComponents(4);
	vtk_dataset->SetScalarTypeToUnsignedChar();

	vtk_dataset->SetDimensions(dims[0],dims[1],dims[2]);
	vtk_dataset->SetSpacing(steps[0],steps[1],steps[2]);
	vtk_dataset->AllocateScalars();
	
	unsigned char *ptr = (unsigned char*) vtk_dataset->GetScalarPointer();
		
	if (num_grains == 0 && mode==MODE_AVG_EULER) {
		mode = MODE_EULER;
	}else if (num_grains == 0 && mode==MODE_AVG_RF) {
		mode = MODE_RF;
	}
		
	for(i=0;i<nv;i++){
		if(data[i].external){
			ptr[0] = 0;
			ptr[1] = 0;
			ptr[2] = 0;
			ptr[3] = 0;
		}else{
			if (!data[i].orientation.is_non_zero()) {
				e[0] = 0;
				e[1] = 0;
				e[2] = 0;
			} else if (mode == MODE_EULER) { 	
				e = data[i].orientation.euler();
			} else if (mode == MODE_RF) {
				e = data[i].orientation.rf();
				e[0] = e[0]*255;
				e[1] = e[1]*255;
				e[2] = e[2]*255;
			} else if (mode == MODE_AVG_EULER) {
				if (data[i].grain > -1) {
					e = grains[data[i].grain].orientation.euler();
				} else {	
					e[0] = 0;
					e[1] = 0;
					e[2] = 0;
				}
			} else if (mode == MODE_AVG_RF) {
				if (data[i].grain > -1) {
					e = grains[data[i].grain].orientation.rf();
					e[0] = e[0]*255;
					e[1] = e[1]*255;
					e[2] = e[2]*255;
				} else {	
					e[0] = 0;
					e[1] = 0;
					e[2] = 0;
				}
			} else {
				e[0] = 0;
				e[1] = 0;
				e[2] = 0;
			}			 
			
			ptr[0] = (unsigned char)e[0];
			ptr[1] = (unsigned char)e[1];
			ptr[2] = (unsigned char)e[2];
			ptr[3] = (unsigned char)255;
		}
		ptr = ptr + 4;
	}
	
	return vtk_dataset;
}

/// Create a frame to view the data in RF space
vtkObject* Dataset::rf_frame()
{

	int i;
	float e = 2-sqrt((float)2);

	vtkPoints* pts=vtkPoints::New();
	pts->SetNumberOfPoints(num_grains);

	pts->InsertPoint(0, 0,0,0);
	pts->InsertPoint(1, 0,1,0);
	pts->InsertPoint(2, 1,0,0);
	pts->InsertPoint(3, 1,1,0);

	pts->InsertPoint(4, 0,0,1);
	pts->InsertPoint(5, 0,1,1);
	pts->InsertPoint(6, 1,0,1);
	
	pts->InsertPoint(7, 1-e,1,1);
	pts->InsertPoint(8, 1,1-e,1);
	pts->InsertPoint(9, 1,1,1-e);

	vtkLine* lines[15];
	
	for(i=0;i<15;i++){
		lines[i] = vtkLine::New();
	}

	//Base
	lines[ 0]->GetPointIds()->SetId(0, 0);
	lines[ 0]->GetPointIds()->SetId(1, 1);

	lines[ 1]->GetPointIds()->SetId(0, 0);
	lines[ 1]->GetPointIds()->SetId(1, 2);

	lines[ 2]->GetPointIds()->SetId(0, 1);
	lines[ 2]->GetPointIds()->SetId(1, 3);

	lines[ 3]->GetPointIds()->SetId(0, 2);
	lines[ 3]->GetPointIds()->SetId(1, 3);
	
	//Sides
	lines[ 4]->GetPointIds()->SetId(0, 0);
	lines[ 4]->GetPointIds()->SetId(1, 4);

	lines[ 5]->GetPointIds()->SetId(0, 1);
	lines[ 5]->GetPointIds()->SetId(1, 5);
	
	lines[ 6]->GetPointIds()->SetId(0, 2);
	lines[ 6]->GetPointIds()->SetId(1, 6);

	lines[ 7]->GetPointIds()->SetId(0, 3);
	lines[ 7]->GetPointIds()->SetId(1, 9);

	//Top
	lines[ 8]->GetPointIds()->SetId(0, 4);
	lines[ 8]->GetPointIds()->SetId(1, 5);

	lines[ 9]->GetPointIds()->SetId(0, 4);
	lines[ 9]->GetPointIds()->SetId(1, 6);

	lines[10]->GetPointIds()->SetId(0, 5);
	lines[10]->GetPointIds()->SetId(1, 7);
	
	lines[11]->GetPointIds()->SetId(0, 6);
	lines[11]->GetPointIds()->SetId(1, 8);

	//Cut off
	lines[12]->GetPointIds()->SetId(0, 7);
	lines[12]->GetPointIds()->SetId(1, 8);
	
	lines[13]->GetPointIds()->SetId(0, 7);
	lines[13]->GetPointIds()->SetId(1, 9);
	
	lines[14]->GetPointIds()->SetId(0, 8);
	lines[14]->GetPointIds()->SetId(1, 9);
	

	vtkUnstructuredGrid* grid = vtkUnstructuredGrid::New();
	grid->Allocate(1,1);
	grid->SetPoints(pts);

	
	for(i=0;i<15;i++){
		grid->InsertNextCell(lines[0]->GetCellType(), lines[i]->GetPointIds());
	}

	return grid;
	
}

/// Visualise the data in RF space
vtkObject* Dataset::rf_space()
{
	int i;
	float *rf;
	
	vtkVertex* vertex = vtkVertex::New();
	vtkPoints* pts=vtkPoints::New();

	pts->SetNumberOfPoints(num_grains);
		
	for(i=0;i<num_grains;i++){
		rf = grains[i].orientation.rf();
		rf[0]=rf[0];
		rf[1]=rf[1];
		rf[2]=rf[2];
		pts->SetPoint(i,rf);

	}
	
	
	vtkUnstructuredGrid* grid = vtkUnstructuredGrid::New();
	grid->Allocate(1,1);
	grid->SetPoints(pts);

	vtkIntArray* colours = vtkIntArray::New();
	colours->SetName("colours");
	colours->SetNumberOfComponents(1);
	colours->SetNumberOfTuples(num_grains);
	

	for(i=0;i<num_grains;i++){
		vertex->GetPointIds()->SetId(0, i);
		colours->InsertValue(i, i);
		grid->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
	}

	grid->GetCellData()->SetScalars(colours);

	printf("made space\n");
	
	return grid;
	
	
}

/// Produce a VTK surface representing a grain
/**
	@param grain the grain to visualise
*/
vtkObject* Dataset::grain_surface(int grain)
{

	int i,j,
		index,
		point_count,
		num_vertices,
		num_triangles;
		
	Triangle *triangles;
	
	num_triangles = grains[grain].num_triangles;
	triangles = grains[grain].surface_triangles;
	

	num_vertices = num_triangles*3;

	vtkPoints* pts=vtkPoints::New();
	pts->SetNumberOfPoints(num_vertices);
	
	vtkCellArray* gtriangles = vtkCellArray::New();
	int size = gtriangles->EstimateSize(num_triangles,3);
	gtriangles->Allocate(10, 5);

	int *ptr = (int*) gtriangles->WritePointer(num_triangles,size);
	
	int* point_map = (int*) malloc(sizeof(int)*nv);
	if (point_map == NULL) { return 0; }
	
	for(i=0;i<nv;i++){
		point_map[i] = -1;
	}
	
	// VTK wants points referenced by number with no duplicates
	for(i=0;i<num_triangles;i++){
		*(ptr++) = 3;		
		for(j=0;j<3;j++){
			
			index = triangles[i].p[j].x + triangles[i].p[j].y*dy + triangles[i].p[j].z*dz;	
			if (point_map[index] == -1) {
				pts->SetPoint(point_count, (double)triangles[i].p[j].x*steps[X],
				   				 		   (double)triangles[i].p[j].y*steps[Y],
								 		   (double)triangles[i].p[j].z*steps[Z]);
				
				*(ptr++) = point_count;
				point_map[index] = point_count++;
				assert(point_count < num_vertices);
			} else {
				*(ptr++) = point_map[index];
			}
		}	
	}
		
	free(point_map);

	//Create vtk Triangles
	//Associate triangles with grains

	vtkPolyData* surface = vtkPolyData::New();
	surface->SetPoints(pts);
	surface->SetPolys(gtriangles);
	
	surface->GetCellData();

	free(triangles);	
	pts->Delete();
	gtriangles->Delete();

	return surface;
	
}


/// Create a lookup table for the grain colours, mapping RF->RGB
vtkLookupTable* Dataset::build_lut() 
{
	
	int i;
	float *r;
	vtkLookupTable* lut = vtkLookupTable::New(); 
	lut->SetNumberOfColors(num_grains+1);
	lut->SetTableRange(0,num_grains);
	
	for(i=0;i<num_grains;i++){
		r = grains[i].orientation.rf();
		lut->SetTableValue(i,r[0],r[1],r[2],1);
	}

	lut->SetTableValue(num_grains,1,0,0,1);

	return lut;
	
}

/// Create a VTK surface of a grain coloured by the orientation of a its neighbours 
vtkPolyData* Dataset::adj_grain_surface(int grain)
{
	int i,j,k;
	int x,y,z;
	int num_quads = 0, new_quads = 0, buffer_count = 1;
	int num_vertices = 0;

	Quad *quads;
	XYZ p;
	
	Point *d;
	Grain *g;

	g = grains+grain;
	
	quads = (Quad*) malloc(sizeof(Quad)*500); //quad_BUFFER);
	if (quads == NULL) { return 0; }

	class GrainList *list;
	list = g->list_head;
	for(list=g->list_head;list->next!=NULL;list=list->next){
		d = data + list->index;	
		
		p.x = (list->index/dx) % tx;
		p.y = (list->index/dy) % ty;
		p.z = (list->index/dz) % tz;

		x = p.x;
		y = p.y;
		z = p.z;

		if(num_quads+24 > buffer_count*500) {
			quads = (Quad*) realloc(quads, sizeof(Quad)*(++buffer_count)*500);
			if (quads == NULL) { return 0; }
		}
		if(x==0){
			make_patch(p,X,-1,quads+num_quads, steps,num_grains);
			num_quads++;
		}
		if(x==tx-1){
			make_patch(p,X,+1,quads+num_quads, steps,num_grains);
			num_quads++;
		}
		if(y==0){
			make_patch(p,Y,-1,quads+num_quads, steps,num_grains);
			num_quads++;
		}
		if(y==ty-1){
			make_patch(p,Y,+1,quads+num_quads, steps,num_grains);
			num_quads++;
		}
		if(z==0){
			make_patch(p,Z,-1,quads+num_quads, steps,num_grains);
			num_quads++;
		}
		if(z==tz-1){
			make_patch(p,Z,+1,quads+num_quads, steps,num_grains);
			num_quads++;
		}

		if(x>0 && (d-dx)->grain != d->grain){
			make_patch(p,X,-1,quads+num_quads, steps,(d-dx)->grain);
			num_quads++;
		}
		if(x<tx-1 && (d+dx)->grain != d->grain){
			make_patch(p,X,+1,quads+num_quads, steps,(d+dx)->grain);
			num_quads++;
		}

		if(y>0 && (d-dy)->grain != d->grain){
			make_patch(p,Y,-1,quads+num_quads, steps,(d-dy)->grain);
			num_quads++;
		}
		if(y<ty-1 && (d+dy)->grain != d->grain){
			make_patch(p,Y,+1,quads+num_quads, steps,(d+dy)->grain);
			num_quads++;
		}
		
		if(z>0 && (d-dz)->grain != d->grain){
			make_patch(p,Z,-1,quads+num_quads, steps,(d-dz)->grain);
			num_quads++;
		}
		if(z<tz-1 && (d+dz)->grain != d->grain){
			make_patch(p,Z,+1,quads+num_quads, steps,(d+dz)->grain);
			num_quads++;
		}		
		
	}	

	printf("Number of quads: %i\n",num_quads);

	
	vtkPoints* pts=vtkPoints::New();
	pts->SetNumberOfPoints(num_quads*4);
	
	vtkCellArray* gquads = vtkCellArray::New();
	int size = gquads->EstimateSize(num_quads,4);
	gquads->Allocate(10, 5);

	int *ptr = (int*) gquads->WritePointer(num_quads,size);
	

	vtkIntArray* colours = vtkIntArray::New();
	colours->SetName("colours");
	colours->SetNumberOfComponents(1);
	colours->SetNumberOfTuples(num_quads);
	
	k=0;	
	for(i=0;i<num_quads;i++){
		colours->InsertValue( i, quads[i].grain1 );
		*(ptr++) = 4;		
		for(j=0;j<4;j++){
			*(ptr++) = k;
			pts->SetPoint(k, (double)quads[i].p[j].x,
			   				 (double)quads[i].p[j].y,
							 (double)quads[i].p[j].z);
			k++;
		}	
	}
		
	//Create vtk quads
	//Associate quads with grains

	vtkPolyData* surface = vtkPolyData::New();
	surface->SetPoints(pts);
	surface->SetPolys(gquads);
	surface->GetCellData()->SetScalars(colours);
	
	free(quads);	
	pts->Delete();
	gquads->Delete();
		
	return surface;

}

/// Make a Quad patch on the face between two voxels
/**
	@param XYZ position of the centre of the patch
	@param axis normal to the patch
	@quad where to store the returned quad
	@grain1 grain of 1st voxel
	@grain2 grain of 2nd voxel
*/
void make_patch(XYZ p, int axis, int dir, Quad *quad, float steps[], int grain1, int grain2)
{
	XYZ v1,v2,v3,v4;

	int a1 = axis;
	int a2 = (axis+1) % 3;
	int a3 = (axis+2) % 3;
	
	v1.p[a1] = (p.p[a1] + dir*0.5)*steps[a1];
	v1.p[a2] = (p.p[a2]+0.5)*steps[a2];
	v1.p[a3] = (p.p[a3]+0.5)*steps[a3];

	v2.p[a1] = v1.p[a1];
	v2.p[a2] = (p.p[a2]+0.5)*steps[a2];
	v2.p[a3] = (p.p[a3]-0.5)*steps[a3];

	v3.p[a1] = v1.p[a1];
	v3.p[a2] = (p.p[a2]-0.5)*steps[a2];
	v3.p[a3] = (p.p[a3]-0.5)*steps[a3];

	v4.p[a1] = v1.p[a1];
	v4.p[a2] = (p.p[a2]-0.5)*steps[a2];
	v4.p[a3] = (p.p[a3]+0.5)*steps[a3];

	quad->p[0] = v1;
	quad->p[1] = v2;
	quad->p[2] = v3;
	quad->p[3] = v4;
	quad->grain1 = grain1;
	quad->grain2 = grain2;

}
