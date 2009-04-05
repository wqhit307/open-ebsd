#ifndef _EBSDH
#include "../ebsd.h"
#endif

#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkOpenGLRenderer.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkInteractorStyleFlight.h"
#include "vtkImageMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkImageDataGeometryFilter.h"

#include "vtkStructuredPoints.h"
#include "vtkImagePlaneWidget.h"
#include "wxVTKRenderWindowInteractor.h"

class vtk3DEBSDWindow : public wxVTKRenderWindowInteractor
{
private:
	vtkRenderer       	*pRenderer;
	vtkStructuredPoints *pDataPoints;
	vtkImageDataGeometryFilter* pFilter[3];
	vtkPolyDataMapper	*pPlane[3];
	vtkActor			*pActor[3];
	
	int slice[3];
	bool init;
	
	Dataset *ds;
	
	void UpdateExtents();
	void ConfigureSlices();
	
public:
	void ConstructVTK();
    void DestroyVTK();
    
    void SetDataset( Dataset* dataset );
   	void DisplaySlices();
   	void SetSlices( int x, int y, int z );
    void RefreshData();

    vtk3DEBSDWindow( wxWindow* parent, wxWindowID id );
};
