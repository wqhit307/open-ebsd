/** 
	\file pymod.cpp
	\brief Define the python wrapper
*/


/************************************************************/
/* Define a new python object, representing a ebsd dataset  */
/************************************************************/
#include "pymod.h"

//--------------------------------------------------
// Python object declaration						//
//--------------------------------------------------

PyTypeObject ebsd_Type = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,
  "ebsd",		   			/* char *tp_name; */
  sizeof(ebsd),			 	/* int tp_basicsize; */
  0,						/* int tp_itemsize;	   /* not used much */
  ebsd_dealloc,				/* destructor tp_dealloc; */
  0,						/* printfunc  tp_print;   */
  ebsd_getattr,				/* getattrfunc  tp_getattr; /* __getattr__ */
  //counter_setattr,		/* setattrfunc  tp_setattr;  /* __setattr__ */
  0,		  				/* setattrfunc  tp_setattr;  /* __setattr__ */
  0,		  				/* cmpfunc  tp_compare;  /* __cmp__ */
  0,						/* reprfunc  tp_repr;	/* __repr__ */
  0,						/* PyNumberMethods *tp_as_number; */
  0,						/* PySequenceMethods *tp_as_sequence; */
  0,						/* PyMappingMethods *tp_as_mapping; */
  0,						/* hashfunc tp_hash;	 /* __hash__ */
  0,						/* ternaryfunc tp_call;  /* __call__ */
  0,						/* reprfunc tp_str;	  /* __str__ */
};

//--------------------------------------------------
// Creation and destruction methods					//
//--------------------------------------------------

static PyObject* 
ebsd_NEW()
{ 
	ebsd *object = NULL;	
	object = PyObject_New(ebsd, &ebsd_Type);
	object->opened = false;
	object->callback = NULL;

	return (PyObject*) object;
 }

static void 
ebsd_dealloc(PyObject* self)
{
	PyObject_Del(self);
}

/* this is the Python constructor, as opposed to the C constructor */
/* this is a normal module function */
static PyObject *
ebsd_new(PyObject* self, PyObject* args)
{ 
	PyObject *object = NULL;
	object = ebsd_NEW();
	return object;
 }

//--------------------------------------------------
// Data attribute methods							//
//--------------------------------------------------
static PyObject*
ebsd_getattr(PyObject* self, char* attrname)
{ 
	ebsd *ds = (ebsd*)self;
	PyObject *result = NULL;
	
	if (strcmp(attrname, "num_grains") == 0) {
		result = PyInt_FromLong(ds->d.num_grains);

    } else if(strcmp(attrname, "dims") == 0) {
    	result = Py_BuildValue("(i,i,i)", ds->d.dims[0], 
										  ds->d.dims[1], 
										  ds->d.dims[2]);
    
    } else if(strcmp(attrname, "steps") == 0) {
    	result = Py_BuildValue("(f,f,f)", ds->d.steps[0], 
										  ds->d.steps[1], 
										  ds->d.steps[2]);
    }
    
    else {
		result = Py_FindMethod(ebsd_methods, self, attrname);
	}
	
	return result;
}
 
//--------------------------------------------------
// Methods											//
//--------------------------------------------------
 
// Set progress callback
//--------------------------------------------------
static PyObject *
ebsd_set_progress_callback(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	if (PyArg_ParseTuple(args, "O:set_callback", &(ds->callback))){
		if(!PyCallable_Check(ds->callback)){
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			ds->callback = NULL;
			return NULL;
		}
		printf("...set callback\n");
		Py_XINCREF(ds->callback);
	} else {
		ds->callback = NULL;
		return NULL;
	}
	return Py_BuildValue("i",1);
}

// Read raw file
//--------------------------------------------------
static PyObject *
ebsd_read_raw(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	const char *file_name;
	int rtn;
	
	if (!PyArg_ParseTuple(args, "s", &file_name))
		return NULL;

	if (!ds->opened) {
		ds->d = Dataset(file_name,ds->callback);
		rtn = ds->d.init;
		ds->opened = ds->d.init;
	} else {
		rtn = 0;
	}
	
	printf("a\n");
	callback_finished(ds->callback);
	return Py_BuildValue("i", rtn);	
} 

// Write raw
//--------------------------------------------------
static PyObject *
ebsd_write_raw(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	const char *file_name;
	int rtn;
	
	if (!PyArg_ParseTuple(args, "s", &file_name))
		return NULL;

	rtn = ds->d.write_raw(file_name);

	return Py_BuildValue("i", rtn);	
} 

// Read Channel5 file
//--------------------------------------------------
static PyObject *
ebsd_read_channel5(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	const char *file_name;
	int rtn = 0;
	
	int dims[3];
	float steps[3];
	if (!PyArg_ParseTuple(args, "siiifff",  &file_name, 
											dims, 
											dims+1, 
											dims+2,
											steps,
											steps+1,
											steps+2))
		return NULL;
	
	if (!ds->opened) {
		if (dims[0] > 0 && dims[1] > 0 && dims[2] > 0) {
			ds->d = Dataset(file_name,dims,steps,ds->callback);
			rtn = ds->d.init;
			ds->opened = ds->d.init;
		}
	}
	callback_finished(ds->callback);
	return Py_BuildValue("i", rtn);	
} 

// Grain Surface
//--------------------------------------------------
static PyObject *
ebsd_grain_surface(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int grain;
	PyObject *pyVTK;
	vtkObject *obj;
	
	if (!PyArg_ParseTuple(args, "i", &grain))
		return NULL;
	
//	if (ds->d.init) {
		obj = ds->d.grain_surface(grain);
		pyVTK = vtkPythonGetObjectFromPointer(obj);
		return pyVTK;
//	} else {
//		return Py_BuildValue("i",0);
//	}
} 

// Adj Grain Surface
//--------------------------------------------------
static PyObject *
ebsd_adj_grain_surface(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int grain;
	PyObject *pyVTK;
	vtkObject* obj;
	
	if (!PyArg_ParseTuple(args, "i", &grain))
		return NULL;
	
	if (ds->d.init) {
		obj = ds->d.adj_grain_surface(grain);
		pyVTK = vtkPythonGetObjectFromPointer(obj);
		return pyVTK;
	} else {
		return Py_BuildValue("i",0);
	}
} 

// Grain LUT
//--------------------------------------------------
static PyObject *
ebsd_grain_lut(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;

	vtkObject* obj;
	PyObject *pyVTK;
	
	if (ds->d.init) {
		obj = (vtkObject*) ds->d.build_lut();
		pyVTK = vtkPythonGetObjectFromPointer(obj);
		return pyVTK;
	} else {
		return Py_BuildValue("i",0);
	}
} 

// Boundary LUT
//--------------------------------------------------
static PyObject *
ebsd_boundary_lut(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;

	vtkObject* obj;
	PyObject *pyVTK;
	
	if (ds->d.init) {
		obj = (vtkObject*) ds->d.boundary_lut();
		pyVTK = vtkPythonGetObjectFromPointer(obj);
		return pyVTK;
	} else {
		return Py_BuildValue("i",0);
	}
} 

// Boundary Network
//--------------------------------------------------
static PyObject *
ebsd_boundary_network(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	PyObject *pyVTK;
	vtkObject* obj;
		
	if (ds->d.init) {
		obj = ds->d.boundary_network();
		pyVTK = vtkPythonGetObjectFromPointer(obj);
		return pyVTK;
	} else {
		return Py_BuildValue("i",0);
	}
}


// RF Frame
//--------------------------------------------------
static PyObject *
ebsd_rf_frame(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	PyObject *pyVTK;
	vtkObject* obj;
		
	if (ds->d.init) {
		obj = ds->d.rf_frame();
		pyVTK = vtkPythonGetObjectFromPointer(obj);
		return pyVTK;
	} else {
		return Py_BuildValue("i",0);
	}
}


// RF Space
//--------------------------------------------------
static PyObject *
ebsd_rf_space(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	PyObject *pyVTK;
	vtkObject* obj;
		
	if (ds->d.init) {
		obj = ds->d.rf_space();
		pyVTK = vtkPythonGetObjectFromPointer(obj);
		return pyVTK;
	} else {
		return Py_BuildValue("i",0);
	}
}



// VTK Dataset
//--------------------------------------------------
static PyObject *
ebsd_dataset_vis(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int mode;
	PyObject *pyVTK;
	
	if (!PyArg_ParseTuple(args, "i", &mode))
		return NULL;
	
	if (ds->d.init) {
		ds->d.vtk_3d_dataset(mode);
		pyVTK = vtkPythonGetObjectFromPointer(ds->d.vtk_dataset);
		return pyVTK;
	} else {
		return Py_BuildValue("i",0);
	}
} 

// Return grain at a given position
//--------------------------------------------------
static PyObject *
ebsd_grain_at_position(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int grain = -1;
	float x,y,z;
	int xi,yi,zi;
	
	int dx = 1,
		dy = ds->d.dims[X],
		dz = ds->d.dims[X]*ds->d.dims[Y];
	
	if (!PyArg_ParseTuple(args, "(fff)", &x, &y, &z))
		return NULL;
	
	xi = (int) (x/(ds->d.steps[X]));
	yi = (int) (y/(ds->d.steps[Y]));
	zi = (int) (z/(ds->d.steps[Z]));
		
	if (ds->d.num_grains>0 && xi<ds->d.dims[X] && yi<ds->d.dims[Y] && zi<ds->d.dims[Z]) {
		grain = ds->d.data[xi*dx+yi*dy+zi*dz].grain;
	}
	
	return Py_BuildValue("i",grain);
	
} 

//-get_grain_mapper----------------------------------- 
static PyObject *
ebsd_grain_mapper(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	
	PyObject *grain_map;
		
	int	dy = ds->d.dims[X],
		dz = ds->d.dims[X]*ds->d.dims[Y];
	
	int nd = 3;
	npy_intp* d = (npy_intp*)ds->d.dims,
		  	 strides[3] = {sizeof(Point),sizeof(Point)*dy,sizeof(Point)*dz};

	PyArray_Descr *int_descr   = PyArray_DescrFromType(PyArray_INT),
				  *float_descr = PyArray_DescrFromType(PyArray_FLOAT);
	
	if (ds->d.num_grains>0) {
		grain_map = (PyObject*) 
					  PyArray_NewFromDescr(&PyArray_Type,int_descr,nd,
						    			   d,strides,(char*)&(ds->d.data[0].grain),NPY_ALIGNED,NULL);
	} else {
		grain_map = Py_BuildValue("{}");
	}

	return grain_map;
	
} 

// Return grain euler angles/RF
//--------------------------------------------------
static PyObject *
ebsd_grain_representation(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int grain, mode;
	
	if (!PyArg_ParseTuple(args, "ii", &grain, &mode))
		return NULL;
		
	if (mode==MODE_EULER || mode==MODE_AVG_EULER) {
		float *e = ds->d.grains[grain].orientation.euler();
				printf("%f %f %f\n",e[0]/360,e[1]/360,e[2]/360);

		return Py_BuildValue("(f,f,f)", e[0]/360,e[1]/360,e[2]/360);
		
	} else if (mode==MODE_RF || mode==MODE_AVG_RF) {
		float *rf = ds->d.grains[grain].orientation.rf();
		return Py_BuildValue("(f,f,f)", rf[0],rf[1],rf[2]);
	}

}

// Return Grain List
//--------------------------------------------------
static PyObject *
ebsd_grain_list(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	
	PyObject *volume_list, *num_adj_grain_list, *centroid_list, *axis_list, *dict;
	
	int nd = 1;
	npy_intp d[2] = {ds->d.num_grains,0},
		  	 strides[2] = {sizeof(Grain),0};
	
	PyArray_Descr *int_descr   = PyArray_DescrFromType(PyArray_INT),
				  *float_descr = PyArray_DescrFromType(PyArray_FLOAT);
	
	if (ds->d.num_grains>0) {
		volume_list = (PyObject*) 
					  PyArray_NewFromDescr(&PyArray_Type,int_descr,nd,
						    			   d,strides,(char*)&(ds->d.grains[0].volume),NPY_ALIGNED,NULL);
		num_adj_grain_list = (PyObject*) 
					  PyArray_NewFromDescr(&PyArray_Type,int_descr,nd,
						    			   d,strides,(char*)&(ds->d.grains[0].num_adj_grains),NPY_ALIGNED,NULL);
		
		nd = 2;
		d[1] = 3;
		strides[1] = sizeof(float);
		
		centroid_list = (PyObject*) 
					  PyArray_NewFromDescr(&PyArray_Type,float_descr,nd,
						    			   d,strides,(char*)&(ds->d.grains[0].centroid),NPY_ALIGNED,NULL);
		
		axis_list = (PyObject*) 
					  PyArray_NewFromDescr(&PyArray_Type,float_descr,nd,
						    			   d,strides,(char*)&(ds->d.grains[0].axis),NPY_ALIGNED,NULL);
						    			   	
		dict = PyDict_New();
		PyDict_SetItemString(dict, "volume", volume_list);
		PyDict_SetItemString(dict, "num_adj_grains", num_adj_grain_list);
		PyDict_SetItemString(dict, "centroid", centroid_list);
		PyDict_SetItemString(dict, "axis", axis_list);
		
	} else {
		dict = Py_BuildValue("{}");
	}
	
	return dict;
	
} 

// Calulate offsets
//--------------------------------------------------
static PyObject *
ebsd_calculate_offsets(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;

	int rtn,i;		
	rtn = ds->d.calculate_offsets();
	
	callback_finished(ds->callback);
	return Py_BuildValue("i",rtn);	
	
} 

// Get list of suspicious slices
//--------------------------------------------------
static PyObject *
ebsd_suspicious(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int i;
	char s[100];
		
	//Build list of suspicious slices
	PyObject* suspicious = PyList_New(0);
	for (i=1;i<ds->d.dims[Z]-1;i++){
		if (ds->d.suspicious[i]){
			sprintf(s,"%.3f%%",ds->d.probability[i]);	
			PyList_Append(suspicious,Py_BuildValue("(i,s)",i,s));
		}
	}
	
	callback_finished(ds->callback);
	return suspicious;	
	
}

// Align Slices
//--------------------------------------------------
static PyObject *
ebsd_align_slices(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int rtn,i,list_size;
	long m;
	PyObject *mark_list, *n;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &mark_list ))
		return NULL;
		
	list_size = PyList_Size(mark_list);

	for (i=1;i<ds->d.dims[Z]-1;i++){
		ds->d.marked[i] = false;
	}

	for(i=0;i<list_size;i++){
		n = PyList_GetItem(mark_list,i);
		m = PyInt_AsLong(n);
		ds->d.marked[m] = true;
	}
	
	rtn = ds->d.calculate_offsets();
	rtn = ds->d.realign_slices();
	
	callback_finished(ds->callback);
	return Py_BuildValue("i", rtn);	
}

// Segment
//--------------------------------------------------
static PyObject *
ebsd_segment(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int rtn;
	float min_angle;
	
	if (!PyArg_ParseTuple(args, "f", &min_angle))
		return NULL;
		
	rtn = ds->d.segment(min_angle);
	rtn = ds->d.calculate_grain_info();
	
	callback_finished(ds->callback);
	return Py_BuildValue("i", rtn);	
} 

// Reconstruct grains
//--------------------------------------------------
static PyObject *
ebsd_reconstruct(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int rtn;
	if (ds->d.num_grains > 0) {	
		rtn = ds->d.calculate_grain_info();
	} else {
		rtn = 0;
	}
	callback_finished(ds->callback);
	return Py_BuildValue("i", rtn);	
} 

// Dilate grains
//--------------------------------------------------
static PyObject *
ebsd_dilate_grains(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int rtn;
			
	rtn = ds->d.dilate_grains();
	callback_finished(ds->callback);
	return Py_BuildValue("i", rtn);	
} 

// Filter grains
//--------------------------------------------------
static PyObject *
ebsd_filter_grains(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int rtn, critical_volume;
	
	if (!PyArg_ParseTuple(args, "i", &critical_volume))
		return NULL;
		
	rtn = ds->d.filter_grains(critical_volume);
	rtn = ds->d.calculate_grain_info();
	callback_finished(ds->callback);
	return Py_BuildValue("i", rtn);	
}

// Calculate extra stats
//--------------------------------------------------
static PyObject *
ebsd_calculate_extra_stats(PyObject *self, PyObject *args)
{
	ebsd *ds = (ebsd*)self;
	int rtn;
	if (ds->d.num_grains > 0) {	
		rtn = ds->d.calculate_extra_stats();
	} else {
		rtn = 0;
	}
	callback_finished(ds->callback);
	return Py_BuildValue("i", rtn);	
} 

PyMODINIT_FUNC
initebsd(void)
{
	(void) Py_InitModule("ebsd", methods);
	import_array();
}
