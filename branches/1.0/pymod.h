/** 
	\file pymod.h
	\brief Header file for python wrapper module
*/

#include <vtkPython.h>
#include <vtkPythonUtil.h>

#include "ebsd.h"

#include "arrayobject.h"

#define Counter_Check(v)  ((v)->ob_type == &counter_Type)
#define Counter_value(v)  (((counter *)(v))->value)


/// Wrapper class for Python object
typedef struct {
	PyObject_HEAD
	
	Dataset d; 				//!< Dataset object being wrapped
	bool opened;			//!< Whether a file has been opened yet
	Callback *callback;		//!< Python function callback for progress display
} ebsd;

//--------------------------------------------------//
// Function definitions								//
//--------------------------------------------------//

static PyObject* ebsd_NEW();
static void ebsd_dealloc(PyObject* self);
static PyObject* ebsd_new(PyObject* self, PyObject* args);
static PyObject *ebsd_getattr(PyObject* self, char* attrname);

static PyObject* ebsd_set_progress_callback(PyObject *self, PyObject *args);

static PyObject* ebsd_read_raw(PyObject *self, PyObject *args);
static PyObject* ebsd_write_raw(PyObject *self, PyObject *args);
static PyObject* ebsd_read_channel5(PyObject *self, PyObject *args);

static PyObject* ebsd_dataset_vis(PyObject *self, PyObject *args);
static PyObject* ebsd_grain_surface(PyObject *self, PyObject *args);
static PyObject* ebsd_adj_grain_surface(PyObject *self, PyObject *args);
static PyObject* ebsd_grain_list(PyObject *self, PyObject *args);
static PyObject* ebsd_grain_mapper(PyObject *self, PyObject *args);
static PyObject* ebsd_grain_at_position(PyObject *self, PyObject *args);
static PyObject* ebsd_grain_representation(PyObject *self, PyObject *args);
static PyObject* ebsd_grain_lut(PyObject *self, PyObject *args);

static PyObject* ebsd_rf_space(PyObject *self, PyObject *args);
static PyObject* ebsd_rf_frame(PyObject *self, PyObject *args);


static PyObject* ebsd_align_slices(PyObject *self, PyObject *args);
static PyObject* ebsd_suspicious(PyObject *self, PyObject *args);
static PyObject* ebsd_calculate_offsets(PyObject *self, PyObject *args);

static PyObject* ebsd_segment(PyObject *self, PyObject *args);
static PyObject* ebsd_reconstruct(PyObject *self, PyObject *args);
static PyObject* ebsd_dilate_grains(PyObject *self, PyObject *args);
static PyObject* ebsd_filter_grains(PyObject *self, PyObject *args);
static PyObject* ebsd_calculate_extra_stats(PyObject *self, PyObject *args);

static PyObject* ebsd_boundary_network(PyObject *self, PyObject *args);
static PyObject* ebsd_boundary_lut(PyObject *self, PyObject *args);


//--------------------------------------------------//
// Python method declarations						//
//--------------------------------------------------//  

static PyMethodDef ebsd_methods[] = {
	{"read_channel5",  ebsd_read_channel5, METH_VARARGS,
	 "Read csv exported from Channel 5"},
	{"read_raw",  ebsd_read_raw, METH_VARARGS,
	 "Read raw format file"},
	{"write_raw",  ebsd_write_raw, METH_VARARGS,
	 "Write raw format file"},

	{"dataset_vis",  ebsd_dataset_vis, METH_VARARGS,
	 "Visualise dataset"}, 
	{"grain_surface",  ebsd_grain_surface, METH_VARARGS,
	 "Visualise grain"}, 
	{"adj_grain_surface",  ebsd_adj_grain_surface, METH_VARARGS,
	 "Visualise grain"}, 
	{"grain_lut",  ebsd_grain_lut, METH_VARARGS,
	 "Get grain LUT"}, 
	 
	{"rf_space",  ebsd_rf_space, METH_VARARGS,
	 "Get rf space"}, 
	 
	{"rf_frame",  ebsd_rf_frame, METH_VARARGS,
	 "Get rf frame"}, 
	 
	{"boundary_lut", ebsd_boundary_lut, METH_VARARGS,
	 "Get boundary LUT"}, 
	{"boundary_network",  ebsd_boundary_network, METH_VARARGS,
	 "Get bounary network"}, 
	 
	 
	{"grains",  ebsd_grain_list, METH_VARARGS,
	 "Get grain information"}, 
	{"grain_mapper",  ebsd_grain_mapper, METH_VARARGS,
	 "Get list of grains associated with points"}, 
	{"grain_at_position",  ebsd_grain_at_position, METH_VARARGS,
	 "Get grains associated with point"}, 
	{"grain_representation",  ebsd_grain_representation, METH_VARARGS,
	 "Get euler/rf values associated with grain"}, 
	 
	{"set_progress_callback",  ebsd_set_progress_callback, METH_VARARGS,
	 "Set callback function for progress"}, 

	{"calculate_offsets",  ebsd_calculate_offsets, METH_VARARGS,
	 "Calculate offsets and find suspicious slices"},	 
	{"align_slices",  ebsd_align_slices, METH_VARARGS,
	 "Align slices"},
	{"suspicious",  ebsd_suspicious, METH_VARARGS,
	 "Get list of suspicious slices"},
	 
	{"segment",  ebsd_segment, METH_VARARGS,
	 "Segment data"}, 
	{"reconstruct",  ebsd_reconstruct, METH_VARARGS,
	 "Reconstruct grains"}, 
	{"calculate_extra_stats",  ebsd_calculate_extra_stats, METH_VARARGS,
	 "Calculate equiv ellipsoids and surface areas"}, 

	{"dilate_grains",  ebsd_dilate_grains, METH_VARARGS,
	 "Expand grains to fill empty space"}, 
	{"filter_grains",  ebsd_filter_grains, METH_VARARGS,
	 "Remove small grains"}, 
	 
	 
	{NULL, NULL, 0, NULL}		/* Sentinel */
};

  
static PyMethodDef methods[] = {
	{"ebsd", ebsd_new, METH_VARARGS},
	{NULL, NULL, 0, NULL}		/* Sentinel */
};
