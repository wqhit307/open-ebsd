#ifndef _DIALOGSH_
#define _DIALOGSH_

#include "wxEBSD.h"

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

#endif