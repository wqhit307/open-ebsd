// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/progdlg.h"
#include "wx/rawbmp.h"

#define VTK_FRAME      101
#define MY_VTK_WINDOW 102

#ifndef _EBSDH
#include "../ebsd.h"
#endif

#include "wx-ebsd.h"

#define showProgressDialog(t) ((vtkFrame*)wxWindow::FindWindowById(VTK_FRAME))->progressDialog = new wxProgressDialog(_T("OpenEBSD"), _T(t), VTK_FRAME, this, wxPD_APP_MODAL)
#define clearProgressDialog() ((vtkFrame*)wxWindow::FindWindowById(VTK_FRAME))->progressDialog->Show(FALSE)

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
	void UpdateSlider();
	void UpdateSliderLimits(); 

private:

	Dataset **ds;

public:
	
	vtkFrame(Dataset **dataset);
	virtual void progress( float p );
	wxProgressDialog* progressDialog;
	

};

/** Implementing fileInfoDialogUI */
class fileInfoDialog : public fileInfoDialogUI
{
protected:
	void CheckValues( wxCommandEvent& event );
	void InitDialog();
	void OnOK(wxCommandEvent& event);
	
private:		
	wxString m_dims[3],
			 m_steps[3];

public:
	fileInfoDialog( wxWindow* parent );
	
	int dims[3];
	float steps[3];
	
	
};

class segmentDialog : public segmentDialogUI
{
protected:
	void InitDialog();
	void OnOK(wxCommandEvent& event);
	
private:		
	wxString m_threshold;

public:
	segmentDialog( wxWindow* parent );
	
	float threshold;
	
	
};

class filterDialog : public filterDialogUI
{
protected:
	void InitDialog();
	void OnOK(wxCommandEvent& event);
	
private:		
	wxString m_threshold;

public:
	filterDialog( wxWindow* parent );
	
	int threshold;
	
	
};

class dilateDialog : public dilateDialogUI
{
protected:
	void InitDialog();
	void OnOK(wxCommandEvent& event);
	
private:		
	wxString m_threshold;

public:
	dilateDialog( wxWindow* parent );
	
	float threshold;
	
	
};


class alignmentFrame : public alignmentFrameUI
{
private:
	void Init( wxActivateEvent& event );	
	void UpdateSlice( wxScrollEvent& event );
	void SliceSelected( wxCommandEvent& event );
	void OnOK( wxCommandEvent& event );
	void OnCancel( wxCommandEvent& event );
	
	void UpdateSlice();

	bool init;
	int slice;
	Dataset* ds;

public:
	alignmentFrame( wxWindow* parent, Dataset* dataset );
	
};
