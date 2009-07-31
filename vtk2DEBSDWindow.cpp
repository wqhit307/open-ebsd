#include "vtkFrame.h"
//#include "vtk2DEBSDWindow.h"

#include "wx/rawbmp.h"

int min(int a, int b) { return (a < b ? a : b); }

vtk2DEBSDWindow::vtk2DEBSDWindow(wxWindow *window, Dataset *dataset) : wxPanel(window)
{
	ds = dataset;
	slice = 0;
	this->Connect(wxEVT_PAINT, wxPaintEventHandler(vtk2DEBSDWindow::OnPaint));
	this->Connect(wxEVT_SIZE, wxSizeEventHandler(vtk2DEBSDWindow::OnSize));
	
}

void vtk2DEBSDWindow::SetDataset(Dataset *dataset)
{
	ds = dataset;	
}

void vtk2DEBSDWindow::SetSlice(int s)
{
	slice = s;
	Refresh(false);	
}

void vtk2DEBSDWindow::OnPaint(wxPaintEvent& event)
{
		if (ds!=NULL && ds->init) {
		
		int clientWidth, clientHeight,
			scaleFactor, dx, dy,
			displayWidth, displayHeight;
		GetSize(&clientWidth,&clientHeight);
		
		dx = ds->dims[X];
		dy = ds->dims[Y];
		
		scaleFactor = min(clientWidth/dx, clientHeight/dy);
		displayHeight = dx*scaleFactor;
		displayWidth = dy*scaleFactor;
		
		wxBitmap bmp( dx, dy, -1 );


		wxImage image = bmp.ConvertToImage();
	 
		Point *d = ds->data + slice*(dx*dy);
		
		int rgb[3];
	 
		unsigned char* p = image.GetData(); 

		for ( int y = 0; y < ds->dims[Y]; ++y ){
			for ( int x = 0; x < ds->dims[X]; ++x, ++d ){
				d->map_rgb(MODE_EULER,rgb,ds->grains);
				
				*(p++) = (unsigned char) rgb[R];
				*(p++) = (unsigned char) rgb[G];
				*(p++) = (unsigned char) rgb[B];
			}
		}
		
		//Resize bitmap		
		wxBufferedPaintDC dc(this);
		
		wxColor backColor;
		
		backColor.Set(wxT("#ffffff")); 
		dc.SetBrush(wxBrush(backColor));
		dc.DrawRectangle(0,0,clientWidth,clientHeight);
		
		dc.DrawBitmap( wxBitmap( image.Scale(displayWidth,displayHeight) ),
			(clientWidth-displayWidth)/2,(clientHeight-displayHeight)/2, FALSE);
	}
}

void vtk2DEBSDWindow::OnSize(wxSizeEvent& event)
{
	Refresh(false);	
}


void vtk2DEBSDWindow::Paint()
{}
