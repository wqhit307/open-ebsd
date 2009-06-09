/**
	\file ebsd.h
	\brief Main Class declarations 
*/

/** 
	\mainpage EBSD Reference
	
	This is the documentation for the EBSD Software developed as part of the Part III Physics Project
	'Analysis and visualisation of three dimensional electron back-scattered diffraction datasets'.
	
	The documentation and can be browsed using the tabs at the top of this page. 
	
	The build process is managed by <a href="http://www.cmake.com">CMake</a>, using the configuration file CMakeLists.txt in the source folder.
	
	The Python GUI can be found in the python/ folder.
	
*/

#ifndef _EBSDH_
#define _EBSDH_ 

//--------------------------------------------------
// Standard Headers				 	
//--------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <float.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>

#ifdef COMPILE_WITH_OMP
#include <omp.h>
#endif

#ifndef isnan
#define isnan(x) ((x) != (x))
#endif

//--------------------------------------------------
// Include Newmat Matrix Library
//--------------------------------------------------

#define WANT_STREAM                  // include.h will get stream fns
#define WANT_MATH                    // include.h will get math fns
                                     // newmatap.h will get include.h
#include "newmatap.h"                // need matrix applications
#include "newmatio.h"                // need matrix output routines

#ifdef use_namespace
using namespace NEWMAT;              // access NEWMAT namespace
#endif

typedef double Real;

#include "vtk.h"
#include "visualisation.h"


//--------------------------------------------------
// Convienience definitions		 	
//--------------------------------------------------

/// Different modes
enum MODES { MODE_EULER, MODE_RF, MODE_MAD, MODE_GRAIN, MODE_AVG_EULER, MODE_AVG_RF };
enum COORDS {X, Y, Z, W};
enum RGB {R, G, B};

//--------------------------------------------------
// Constants			
//--------------------------------------------------

#define EULER_TOL	3
#define MAD_THRESHOLD  0.2
#define NUM_NN 26
#define TRIANGLE_BUFFER	2000

#define	NUM_COLUMNS	12
#define EULER_COL 	3
#define MAD_COL		6


class CallbackHandler
{
public:
	virtual void progress(float p, char* t) {};
};

//--------------------------------------------------
// Python Bindings				 
//--------------------------------------------------

#ifdef PYTHON_BINDINGS
	#include "Python.h"
	#define allow_py_threading	Py_BEGIN_ALLOW_THREADS
	#define block_py_threading	Py_END_ALLOW_THREADS
	#define progress_callback(prog, str, finished)	if(callback!=NULL) { PyEval_CallObject(callback,Py_BuildValue("(f,b,s)", (float)prog, finished, str)); }	
	#define callback_finished(c) if(c!=NULL) { PyEval_CallObject(c,Py_BuildValue("(f,b)", 1.0, true)); }
	typedef PyObject Callback;

#else
	//typedef char Callback;
	//typedef void &CallbackHandler;
	#define allow_py_threading
	#define block_py_threading 
//	#define progress_callback(prog, str, finished) printf("\r%s: %i%%",str,(int)(prog*100)); if(prog == 1) { printf("\n"); }
	#define progress_callback(prog, str, finished) callback->progress(prog, str)
#endif


//--------------------------------------------------
// Structure Definitions		 	 
//--------------------------------------------------

typedef float MATRIX[9];
typedef float EULER[3];
typedef float QUAT[4]; // x,y,z,w
typedef float RF[3];

/// Stores information about an adjacent grain
class AdjGrain {
	public:
	int   grain;					//!< Index of the ajacent grain 
	int	  nn_matches;				//!< Number of pairs of voxels adjacent 
	float disorientation;			//!< Disorientation between the grains
};

/**
	\class GrainList
	\brief	Memeber of linked list indexing all the Points in a dataset
*/
class GrainList {
	public:
	int	index;						//!< Index of this point
	GrainList *next;				//!< Pointer to next item in the linked list. Null if last item.
};


/// Represents an orientation
/**
	Uses quaternions internally
*/
class Orientation {
	float e[3];						//!< Euler angles representing orientation
	float r[3];						//!< Rodrigues vector representing orientation
	
	void combine(const float* qB, float* qR);
	
	float combine_W(const float* qR);
	public:
		float q[4];					//!< Quaternion representing the orientation	 
		bool euler_set;				//!< Whether the euler angle representation has been calculated and is still valid

		Orientation(float x, float y, float z, float w);
		Orientation(float *q);
		Orientation(float e_phi1, float e_PHI, float e_phi2);
		Orientation();
		
		void set_q(const float* qR);
		void set_q(const long double* qR);
		
		void invert();
		void reduce_zone();
		void reduce_zone(Orientation *o);
		bool disorientation_threshold(Orientation *o, float threshold);
		float disorientation(Orientation *o) ;
		Orientation misorientation(Orientation *o) ;
		
		bool is_non_zero();
		void set_zero();

		float* euler();
		float* rf();
		
		Orientation operator * (const Orientation o);
		Orientation operator = (const Orientation o);
		Orientation operator = (const long double* qA);
		Orientation operator = (const float* qA);
		
		Orientation operator [] (const int) const;
		Orientation operator [] (const int);

};


/// Represents a grain
class Grain 
{
	public:
	int   	  	 volume;			//!< Volume of the grain in voxels
	float 	  	 surface_area;		//!< Surface area	
	int   	  	 surface_points;	//!< Number of voxels on the surface of grain
	Orientation  orientation;		//!< Surface area
	float	  	 centroid[3];		//!< Location of centre of grain
	float	  	 axis[3];			//!< Principal axes of equivalent ellipsoid
	bool 		 on_edge;			//!< Whether the grain intesects the edge of the dataset
	
	GrainList 	*list_head;			//!< Pointer to first voxel in grain linked list
	GrainList 	*list_tail;			//!< Pointer to last voxel in grain linked list

	AdjGrain 	*adj_grains;		//!< Array of adjacent grains
	int		  	 num_adj_grains;	//!< Number of adjacent grains

	Triangle 	*surface_triangles;	//!< Set of triangles making up surface
	int		 	 num_triangles;		//!< Number of surface triangles
};

/// Represents a point in the dataset 
class Point {
	public:
	float mad;						//!< Confidence measure of the indexing 
	Orientation orientation;		//!< Orientation of the point
	float bc;						//!< Band constrast
	int   grain;					//!< Which grain this point belongs to
	bool  external;					//!< Whether or not this point is outside the dataset
									/*!< Used to mark points which are created during the alignment procedure */

	void set_external();			//!< Mark a point as outside the dataset
	void map_rgb(int mode, int* rgb, Grain* grains);
};

/// Represents a dataset. Wraps all processing code
class Dataset {
	int nv,							//!< Total number of points in dataset
	d[3],							//!< Offset between points in each direction 
	dx, 							//!< Offset between points in x direction = 1
	dy,  							//!< Offset between points in y direction = tx
	dz, 							//!< Offset between points in z direction = tx*ty
	tx,  							//!< Number of points in x direction
	ty, 							//!< Number of points in y direction
	tz, 							//!< Number of points in z direction
	nn_offset_bulk[NUM_NN+1]; 	 	//!< Offsets to nearest neighbours in bulk of grain
	
	GrainList *grain_list_head;	 	//!< Pointer to memory for grain linked list memebers 
	GrainList *grain_list_free;		//!< Next free memory for linked list memeber
	
	void init_params(CallbackHandler *c);
	int init_alignment();
	void init_geometry(int *dimensions = NULL, float *data_steps = NULL);
	inline double calculate_ftheta(Point *s1, Point *s2, int dx, int dy);
	
	public:
	bool  init;						//!< Whether the dataset has been initialised

	int   dims[3];					//!< Dimensions of data
	float steps[3];					//!< Distance between data points in each direction

	int   num_grains;				//!< Number of grains
	Grain *grains;					//!< Array of grains

	Point *data; 					//!< Array of data points
	
	vtkStructuredPoints* vtk_dataset; //!< VTK representation of dataset 

	//Alignment information
	bool  offsets_calculated;		//!< Whether offsets have been calculated for alignment
	int   *offset_x;				//!< Array of offsets in x direction
	int   *offset_y;				//!< Array of offsets in y direction
	float *probability;				//!< Probability that a slice is distorted
	bool  *suspicious;				//!< Whether a slice has is likely to have been distorted 
	bool  *marked;					//!< Whether a slice has been marked for replacement
		
	CallbackHandler *callback;				//!< Callback for Python interface

	Dataset(const char name[], int dims[], float steps[], CallbackHandler *c);
	Dataset(const char name[], CallbackHandler *c);

	// fileaccess.cpp
	//--------------------------------------------------
	int read_channel5(const char name[]);
	int write_am(const char name[]);
	int read_raw(const char name[]);
	int write_raw(const char name[]);

	int write_ppm(const char name[], int mode);
	int write_csv(const char name[]);
		
	// alignment.cpp
	//--------------------------------------------------
	int calculate_offsets();
	int realign_slices();

	// noise.cpp
	//--------------------------------------------------
	int filter_grains(int critical_volume);
	int merge_grains(float min_angle);
	int dilate_grains();
	int replace_slice(int slice);
	
	// segmentation.cpp
	//--------------------------------------------------
	int segment(float min_angle);
	void nearest_neigbours(int index, int **nn, int *nn_edge, int *num_nn, int mode=0);
	int reconstruct_grains();
	int adjacency_list();

	// characterisation.cpp
	//--------------------------------------------------
	int calculate_grain_info();
	int calculate_extra_stats();
	int average_orientations();
	int calculate_ellipses();
	int calculate_surface();

	// visualisation.cpp
	//--------------------------------------------------
	vtkLookupTable* build_lut(); 
	vtkObject* vtk_3d_dataset(int mode);
	vtkObject* grain_surface(int grain);
	vtkPolyData* adj_grain_surface(int grain);
	
	vtkObject* rf_frame();
	vtkObject* rf_space();
	
	// boundaries.cpp
	//--------------------------------------------------
	vtkLookupTable* boundary_lut();
	vtkPolyData* boundary_network();

};

// visualisation.cpp
//--------------------------------------------------
XYZ VertexInterp(double isolevel, XYZ p1, XYZ p2);
XYZ VertexInterp(XYZ p1, XYZ p2);
int polygonise(Gridcell grid, int grain, Triangle *triangles);
void make_patch(XYZ p, int axis, int dir, Quad *quad, float steps[], int grain1, int grain2=-1);
int choose_grain(int grain,int g1,int g2);


// symmetry.cpp
//--------------------------------------------------
#include "symmetry.h"

//--------------------------------------------------
// Suppress Visual C++ Warnings
//--------------------------------------------------
#pragma warning( disable : 4996 4244 4305 )

#endif