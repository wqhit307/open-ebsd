#ifndef __open-ebsdvtkFrameUI__
#define __open-ebsdvtkFrameUI__

/**
@file
Subclass of vtkFrameUI, which is generated by wxFormBuilder.
*/

#include "wx-ebsd.h"

/** Implementing vtkFrameUI */
class open-ebsdvtkFrameUI : public vtkFrameUI
{
protected:
	// Handlers for vtkFrameUI events.
	void OpenCSVFile( wxCommandEvent& event );
	void OpenRawFile( wxCommandEvent& event );
	void SaveRawFile( wxCommandEvent& event );
	void SliceAlignment( wxCommandEvent& event );
	void Segment( wxCommandEvent& event );
	void Dilate( wxCommandEvent& event );
	void Filter( wxCommandEvent& event );
	void CalculateExtraStats( wxCommandEvent& event );
	void UpdateSlider( wxScrollEvent& event );
	void HideButton( wxCommandEvent& event );
	
public:
	/** Constructor */
	open-ebsdvtkFrameUI( wxWindow* parent );
};

#endif // __open-ebsdvtkFrameUI__