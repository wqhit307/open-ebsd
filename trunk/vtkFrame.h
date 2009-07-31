// For compilers that support precompilation, includes "wx/wx.h".
// #include "wx/wxprec.h"
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)


#ifndef _VTKFRAMEH_
#define _VTKFRAMEH_

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/progdlg.h"

#define VTK_FRAME     101
#define MY_VTK_WINDOW 102

#include "ebsd.h"
#include "wxEBSD.h"

#include "dialogs.h"

#define showProgressDialog(t) ((vtkFrame*)wxWindow::FindWindowById(VTK_FRAME))->progressDialog = new wxProgressDialog(_T("OpenEBSD"), _T(t), VTK_FRAME, this, wxPD_APP_MODAL)
#define clearProgressDialog() ((vtkFrame*)wxWindow::FindWindowById(VTK_FRAME))->progressDialog->Show(FALSE);

class vtkFrame : public vtkFrameUI, CallbackHandler
{

protected:
	
	void OpenRawFile( wxCommandEvent& event );
	void OpenCSVFile( wxCommandEvent& event );
	void SaveRawFile( wxCommandEvent& event );
	void SliceAlignment( wxCommandEvent& event );
	
	void Segment( wxCommandEvent& event );
	void Filter( wxCommandEvent& event );
	void Dilate( wxCommandEvent& event );
	void CalculateExtraStats( wxCommandEvent& event );
	
	void xShowButton( wxCommandEvent& event );
	void xHideButton( wxCommandEvent& event );
	
	void UpdateSlider( wxScrollEvent& event );
	
	void UpdateMenuItems();
	void UpdateSlider( bool setCentre = FALSE );
	void UpdateSliderLimits(); 

private:

	Dataset **ds;

public:
	
	vtkFrame(Dataset **dataset);
	virtual void progress( float p, char* t );
	wxProgressDialog* progressDialog;
	
};

#endif