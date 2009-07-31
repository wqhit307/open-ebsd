#ifndef _ALIGNMENTFRAMEH_
#define _ALIGNMENTFRAMEH_

#include "wxEBSD.h"



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

#endif