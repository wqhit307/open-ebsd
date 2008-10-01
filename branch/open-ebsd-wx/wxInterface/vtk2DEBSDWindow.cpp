#include "interface.h"
//#include "vtk2DEBSDWindow.h"

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
	Paint();	
}

void vtk2DEBSDWindow::OnPaint(wxPaintEvent& event)
{
	Paint();	
}

void vtk2DEBSDWindow::OnSize(wxSizeEvent& event)
{
	Paint();	
}


void vtk2DEBSDWindow::Paint()
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
		wxNativePixelData data(bmp);
	 
		Point *d = ds->data + slice*(dx*dy);
		
		int rgb[3];
	 
		wxNativePixelData::Iterator p(data);

		for ( int y = 0; y < ds->dims[Y]; ++y ){
			wxNativePixelData::Iterator rowStart = p;

			for ( int x = 0; x < ds->dims[X]; ++x, ++p, ++d ){
				d->map_rgb(MODE_EULER,rgb,ds->grains);
				
				p.Red() = rgb[R];
				p.Green() = rgb[G];
				p.Blue() = rgb[B];
			}

			p = rowStart;
			p.OffsetY(data, 1);
		}
		
		//Resize bitmap
		wxImage image = bmp.ConvertToImage();
		
		wxBufferedPaintDC dc(this);
		
		wxColor backColor;
		
		backColor.Set(wxT("#ffffff")); 
		dc.SetBrush(wxBrush(backColor));
		dc.DrawRectangle(0,0,clientWidth,clientHeight);
		
		dc.DrawBitmap( wxBitmap( image.Scale(displayWidth,displayHeight) ),
			(clientWidth-displayWidth)/2,(clientHeight-displayHeight)/2, FALSE);
	}
}
