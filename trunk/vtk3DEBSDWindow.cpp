#include "vtkFrame.h"
#include <vtkInteractorStyleSwitch.h>

vtk3DEBSDWindow::vtk3DEBSDWindow( wxWindow* parent, wxWindowID id ) : wxVTKRenderWindowInteractor( parent, id )
{
    UseCaptureMouseOn();

  	pRenderer = vtkOpenGLRenderer::New();
  	
  	pDataPoints = NULL;
  	
  	for(int i=X; i<=Z; i++){
		pFilter[i] = vtkImageDataGeometryFilter::New();
		pPlane[i] = vtkPolyDataMapper::New();
		pActor[i] = vtkActor::New();
	  	slice[i] = 0;
	}	
  	
	// Enter trackball mode
	vtkInteractorStyleSwitch* interactorStyle = vtkInteractorStyleSwitch::New();
	interactorStyle->SetCurrentStyleToTrackballCamera();
	this->SetInteractorStyle(interactorStyle);
	interactorStyle->Delete();

  	// connect renderer and render window and configure render window
  	GetRenderWindow()->AddRenderer(pRenderer);
  	pRenderer->SetBackground(0.7,0.7,0.7);
  	
  	ds = 0;
  	init = false;
	mode = MODE_EULER;
	
}

vtk3DEBSDWindow::~vtk3DEBSDWindow()
{
	DestroyVTK();
}

void vtk3DEBSDWindow::SetDataset( Dataset* dataset )
{
	ds = dataset;
	RefreshData();	
}

void vtk3DEBSDWindow::SetMode(int mode)
{
	this->mode = mode;

	RefreshData();
}

void vtk3DEBSDWindow::RefreshData()
{
	if (ds->init) {
		pDataPoints = (vtkStructuredPoints*)ds->vtk_3d_dataset(mode);
		
		pFilter[X]->SetInput(pDataPoints); 
		pFilter[Y]->SetInput(pDataPoints); 
		pFilter[Z]->SetInput(pDataPoints); 

		UpdateExtents();
		ConfigureSlices();
	}
}

void vtk3DEBSDWindow::SetSlices(int x, int y, int z)
{
	slice[X] = x-1;
	slice[Y] = y-1;
	slice[Z] = z-1;
		
	UpdateExtents();
	ConfigureSlices();
}

void vtk3DEBSDWindow::UpdateExtents()
{

int *extent_buffer, extent[6];
	
	if (ds>0 && ds->init) {
		
		extent_buffer = pDataPoints->GetExtent();
		
		for(int i=X; i<=Z; i++) {	

			memcpy(extent,extent_buffer,sizeof(int)*6);
			extent[i*2  ] = slice[i];
			extent[i*2+1] = slice[i];			
			pFilter[i]->SetExtent(extent);

		}
		
		pRenderer->Render();
	}
}

void vtk3DEBSDWindow::ConfigureSlices()
{
	
	if (ds>0 && ds->init) {
		
		for(int i=X; i<=Z; i++){
			pPlane[i]->SetInput(pFilter[i]->GetOutput());
			pActor[i]->SetMapper(pPlane[i]);
			pRenderer->AddActor(pActor[i]);
		}

		pRenderer->ResetCamera();
		init = true;
		
		pRenderer->Render();

		Refresh();
		
	}
}

void vtk3DEBSDWindow::DestroyVTK()
{
/*
for(int i=X; i<=Z; i++)
	{
		pPlane[i]->Delete();
		pActor[i]->Delete();
	}

	pRenderer->Delete();
*/
}
