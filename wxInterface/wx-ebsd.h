///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __wx-ebsd__
#define __wx-ebsd__

#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include "vtk3DEBSDWindow.h"
#include <wx/toolbar.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/checklst.h>
#include "vtk2DEBSDWindow.h"
#include <wx/scrolbar.h>

///////////////////////////////////////////////////////////////////////////

#define ID_OPEN_CSV 1000
#define ID_OPEN_RAW 1001
#define ID_SAVE_RAW 1002
#define ID_QUIT 1003
#define ID_CALCULATE_SLICE_ALIGNMENT 1004

///////////////////////////////////////////////////////////////////////////////
/// Class vtkFrameUI
///////////////////////////////////////////////////////////////////////////////
class vtkFrameUI : public wxFrame 
{
	private:
	
	protected:
		wxStatusBar* m_statusBar1;
		wxMenuBar* mainMenu;
		wxMenu* fileTopMenu;
		wxMenuItem* openCSVMenu;
		wxMenuItem* openRawMenu;
		wxMenuItem* saveRawMenu;
		wxMenuItem* exportStatsMenu;
		wxMenu* process;
		wxMenuItem* sliceAlignmentMenu;
		wxMenuItem* segmentMenu;
		wxMenu* filterTopMenu;
		wxMenuItem* dilateMenu;
		wxMenuItem* filterMenu;
		wxMenu* statsTopMenu;
		wxMenuItem* calculateExtraStatsMenu;
		wxStaticText* m_staticText15;
		wxChoice* visualisationMode;
		wxPanel* m_panel3;
		wxChoice* colourMode;
		wxStaticText* m_staticText17;
		wxSlider* xSlider;
		wxStaticText* m_staticText171;
		wxSlider* ySlider;
		wxStaticText* m_staticText172;
		wxSlider* zSlider;
		wxBoxSizer* grainPropNone;
		wxStaticText* m_staticText32;
		wxBoxSizer* grainProp;
		wxStaticText* m_staticText36;
		wxStaticText* grainNumber;
		wxStaticText* m_staticText38;
		wxStaticText* grainVolume;
		wxStaticText* m_StaticText33;
		wxCheckBox* m_checkBox1;
		wxCheckBox* m_checkBox2;
		wxToolBar* m_toolBar1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OpenCSVFile( wxCommandEvent& event ){ event.Skip(); }
		virtual void OpenRawFile( wxCommandEvent& event ){ event.Skip(); }
		virtual void SaveRawFile( wxCommandEvent& event ){ event.Skip(); }
		virtual void SliceAlignment( wxCommandEvent& event ){ event.Skip(); }
		virtual void Segment( wxCommandEvent& event ){ event.Skip(); }
		virtual void Dilate( wxCommandEvent& event ){ event.Skip(); }
		virtual void Filter( wxCommandEvent& event ){ event.Skip(); }
		virtual void CalculateExtraStats( wxCommandEvent& event ){ event.Skip(); }
		virtual void UpdateSlider( wxScrollEvent& event ){ event.Skip(); }
		
	
	public:
		vtk3DEBSDWindow* vtkRenderWindow;
		vtkFrameUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("OpenEBSD"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~vtkFrameUI();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class fileInfoDialogUI
///////////////////////////////////////////////////////////////////////////////
class fileInfoDialogUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText7;
		wxTextCtrl* dimX;
		wxStaticText* m_staticText8;
		wxTextCtrl* dimY;
		wxStaticText* m_staticText9;
		wxTextCtrl* dimZ;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText71;
		wxTextCtrl* spaceX;
		wxStaticText* m_staticText81;
		wxTextCtrl* spaceY;
		wxStaticText* m_staticText91;
		wxTextCtrl* spaceZ;
		wxPanel* m_panel2;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		fileInfoDialogUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("File Dimensions"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~fileInfoDialogUI();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class dilateDialogUI
///////////////////////////////////////////////////////////////////////////////
class dilateDialogUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText21;
		wxStaticText* m_staticText13;
		wxTextCtrl* disorientationThreshold;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		dilateDialogUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Dilate Grains"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~dilateDialogUI();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class segmentDialogUI
///////////////////////////////////////////////////////////////////////////////
class segmentDialogUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText13;
		wxTextCtrl* disorientationThreshold;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		segmentDialogUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Segment Dataset"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~segmentDialogUI();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class filterDialogUI
///////////////////////////////////////////////////////////////////////////////
class filterDialogUI : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText25;
		wxStaticText* m_staticText13;
		wxTextCtrl* filterThreshold;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		filterDialogUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~filterDialogUI();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class alignmentFrameUI
///////////////////////////////////////////////////////////////////////////////
class alignmentFrameUI : public wxFrame 
{
	private:
	
	protected:
		wxCheckListBox* suspiciousList;
		vtk2DEBSDWindow* sliceWindow;
		wxScrollBar* sliceSlider;
		wxStaticText* sliceLabel;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void Init( wxActivateEvent& event ){ event.Skip(); }
		virtual void SliceSelected( wxCommandEvent& event ){ event.Skip(); }
		virtual void UpdateSlice( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		alignmentFrameUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Slice Alignment"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 647,473 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~alignmentFrameUI();
	
};

#endif //__wx-ebsd__
