#ifndef _VTK2DEBSDWINDOW_
#define _VTK2DEBSDWINDOW_

#include "ebsd.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dcbuffer.h"
#include "wx/rawbmp.h"

class vtk2DEBSDWindow : public wxPanel
{
private:
	Dataset *ds;
	int slice;
	
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void Paint();

public:

	vtk2DEBSDWindow(wxWindow *window, Dataset* dataset = NULL);
	void SetDataset(Dataset* dataset);
	void SetSlice(int s);	
		
};

#endif