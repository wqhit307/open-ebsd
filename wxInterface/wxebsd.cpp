///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxebsd.h"

///////////////////////////////////////////////////////////////////////////

vtkFrameUI::vtkFrameUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 600,400 ), wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	
	m_statusBar1 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	mainMenu = new wxMenuBar( 0 );
	fileTopMenu = new wxMenu();
	openCSVMenu = new wxMenuItem( fileTopMenu, ID_OPEN_CSV, wxString( wxT("Open CSV...") ) + wxT('\t') + wxT("CTRL-O"), wxEmptyString, wxITEM_NORMAL );
	fileTopMenu->Append( openCSVMenu );
	
	openRawMenu = new wxMenuItem( fileTopMenu, ID_OPEN_RAW, wxString( wxT("Open Raw...") ) + wxT('\t') + wxT("CTRL-ALT-O"), wxEmptyString, wxITEM_NORMAL );
	fileTopMenu->Append( openRawMenu );
	
	saveRawMenu = new wxMenuItem( fileTopMenu, ID_SAVE_RAW, wxString( wxT("Save Raw...") ) + wxT('\t') + wxT("CTRL-S"), wxEmptyString, wxITEM_NORMAL );
	fileTopMenu->Append( saveRawMenu );
	
	fileTopMenu->AppendSeparator();
	
	exportStatsMenu = new wxMenuItem( fileTopMenu, wxID_ANY, wxString( wxT("Export Grain Statistics...") ) , wxEmptyString, wxITEM_NORMAL );
	fileTopMenu->Append( exportStatsMenu );
	
	fileTopMenu->AppendSeparator();
	
	wxMenuItem* quit;
	quit = new wxMenuItem( fileTopMenu, ID_QUIT, wxString( wxT("Quit") ) + wxT('\t') + wxT("ALT+F4"), wxEmptyString, wxITEM_NORMAL );
	fileTopMenu->Append( quit );
	
	mainMenu->Append( fileTopMenu, wxT("File") );
	
	process = new wxMenu();
	sliceAlignmentMenu = new wxMenuItem( process, ID_CALCULATE_SLICE_ALIGNMENT, wxString( wxT("Slice Alignment...") ) + wxT('\t') + wxT("CTRL-A"), wxEmptyString, wxITEM_NORMAL );
	process->Append( sliceAlignmentMenu );
	
	segmentMenu = new wxMenuItem( process, wxID_ANY, wxString( wxT("Segmentation...") ) , wxEmptyString, wxITEM_NORMAL );
	process->Append( segmentMenu );
	
	mainMenu->Append( process, wxT("Processing") );
	
	filterTopMenu = new wxMenu();
	dilateMenu = new wxMenuItem( filterTopMenu, wxID_ANY, wxString( wxT("Dilate Grains...") ) , wxEmptyString, wxITEM_NORMAL );
	filterTopMenu->Append( dilateMenu );
	
	filterMenu = new wxMenuItem( filterTopMenu, wxID_ANY, wxString( wxT("Filter Grains...") ) , wxEmptyString, wxITEM_NORMAL );
	filterTopMenu->Append( filterMenu );
	
	mainMenu->Append( filterTopMenu, wxT("Filtering") );
	
	statsTopMenu = new wxMenu();
	calculateExtraStatsMenu = new wxMenuItem( statsTopMenu, wxID_ANY, wxString( wxT("Calculate Ellipsoids, Grain Surfaces") ) , wxEmptyString, wxITEM_NORMAL );
	statsTopMenu->Append( calculateExtraStatsMenu );
	
	mainMenu->Append( statsTopMenu, wxT("Statistics") );
	
	this->SetMenuBar( mainMenu );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	bSizer3->SetMinSize( wxSize( 250,-1 ) ); 
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Visualisation") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, wxT("Display Mode"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer6->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString visualisationModeChoices[] = { wxT("Euler Angles"), wxT("RF Space") };
	int visualisationModeNChoices = sizeof( visualisationModeChoices ) / sizeof( wxString );
	visualisationMode = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, visualisationModeNChoices, visualisationModeChoices, 0 );
	visualisationMode->SetSelection( 0 );
	fgSizer6->Add( visualisationMode, 0, wxALL, 5 );
	
	m_panel3 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer6->Add( m_panel3, 1, wxEXPAND | wxALL, 5 );
	
	wxString colourModeChoices[] = { wxT("By Point"), wxT("By Grain") };
	int colourModeNChoices = sizeof( colourModeChoices ) / sizeof( wxString );
	colourMode = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, colourModeNChoices, colourModeChoices, 0 );
	colourMode->SetSelection( 0 );
	fgSizer6->Add( colourMode, 0, wxALL, 5 );
	
	sbSizer1->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Slice Positions") ), wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, wxT("x"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	bSizer5->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	xSlider = new wxSlider( this, wxID_ANY, 1, 1, 2, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer5->Add( xSlider, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer2->Add( bSizer5, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText171 = new wxStaticText( this, wxID_ANY, wxT("y"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	bSizer51->Add( m_staticText171, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ySlider = new wxSlider( this, wxID_ANY, 1, 1, 2, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer51->Add( ySlider, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer2->Add( bSizer51, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText172 = new wxStaticText( this, wxID_ANY, wxT("z"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText172->Wrap( -1 );
	bSizer52->Add( m_staticText172, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	zSlider = new wxSlider( this, wxID_ANY, 1, 1, 2, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer52->Add( zSlider, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer2->Add( bSizer52, 1, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Selected Grain") ), wxVERTICAL );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	grainPropNone = new wxBoxSizer( wxVERTICAL );
	
	m_staticText32 = new wxStaticText( this, wxID_ANY, wxT("No grain selected"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_staticText32->Wrap( -1 );
	m_staticText32->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 93, 90, false, wxEmptyString ) );
	
	grainPropNone->Add( m_staticText32, 0, wxALL|wxEXPAND, 5 );
	
	bSizer21->Add( grainPropNone, 0, wxEXPAND, 5 );
	
	grainProp = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer14->SetFlexibleDirection( wxBOTH );
	fgSizer14->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText36 = new wxStaticText( this, wxID_ANY, wxT("Grain Number"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText36->Wrap( -1 );
	fgSizer14->Add( m_staticText36, 0, wxALL, 5 );
	
	grainNumber = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	grainNumber->Wrap( -1 );
	fgSizer14->Add( grainNumber, 0, wxALL, 5 );
	
	m_staticText38 = new wxStaticText( this, wxID_ANY, wxT("Volume"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText38->Wrap( -1 );
	fgSizer14->Add( m_staticText38, 0, wxALL, 5 );
	
	grainVolume = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	grainVolume->Wrap( -1 );
	fgSizer14->Add( grainVolume, 0, wxALL, 5 );
	
	grainProp->Add( fgSizer14, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Display Grain Surface") ), wxVERTICAL );
	
	m_StaticText33 = new wxStaticText( this, wxID_ANY, wxT("Coloured by..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticText33->Wrap( -1 );
	sbSizer5->Add( m_StaticText33, 0, wxALL, 5 );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, wxT("Grain orientation"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer5->Add( m_checkBox1, 0, wxALL, 2 );
	
	m_checkBox2 = new wxCheckBox( this, wxID_ANY, wxT("Neighbouring grain orientation"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer5->Add( m_checkBox2, 0, wxALL, 2 );
	
	grainProp->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	bSizer21->Add( grainProp, 1, wxEXPAND, 5 );
	
	sbSizer3->Add( bSizer21, 1, wxEXPAND, 5 );
	
	bSizer3->Add( sbSizer3, 1, wxEXPAND, 5 );
	
	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	vtkRenderWindow = new vtk3DEBSDWindow( this, -1 );
	bSizer2->Add( vtkRenderWindow, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer2 );
	this->Layout();
	m_toolBar1 = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY ); 
	m_toolBar1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_MENU ) );
	
	m_toolBar1->Realize();
	
	
	// Connect Events
	this->Connect( openCSVMenu->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::OpenCSVFile ) );
	this->Connect( openRawMenu->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::OpenRawFile ) );
	this->Connect( saveRawMenu->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::SaveRawFile ) );
	this->Connect( sliceAlignmentMenu->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::SliceAlignment ) );
	this->Connect( segmentMenu->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::Segment ) );
	this->Connect( dilateMenu->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::Dilate ) );
	this->Connect( filterMenu->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::Filter ) );
	this->Connect( calculateExtraStatsMenu->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::CalculateExtraStats ) );
	xSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
}

vtkFrameUI::~vtkFrameUI()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::OpenCSVFile ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::OpenRawFile ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::SaveRawFile ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::SliceAlignment ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::Segment ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::Dilate ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::Filter ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( vtkFrameUI::CalculateExtraStats ) );
	xSlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	xSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	ySlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
	zSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( vtkFrameUI::UpdateSlider ), NULL, this );
}

fileInfoDialogUI::fileInfoDialogUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Dimensions"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	m_staticText3->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Sans") ) );
	
	fgSizer3->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 1, 6, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("x"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer4->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	dimX = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( dimX, 0, wxALL, 5 );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("y"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer4->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	dimY = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( dimY, 0, wxALL, 5 );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("z"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer4->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	dimZ = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( dimZ, 0, wxALL, 5 );
	
	fgSizer3->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, wxT("Spacing"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	m_staticText15->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Sans") ) );
	
	fgSizer3->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 1, 6, 0, 0 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText71 = new wxStaticText( this, wxID_ANY, wxT("x"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	fgSizer41->Add( m_staticText71, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	spaceX = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( spaceX, 0, wxALL, 5 );
	
	m_staticText81 = new wxStaticText( this, wxID_ANY, wxT("y"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText81->Wrap( -1 );
	fgSizer41->Add( m_staticText81, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	spaceY = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( spaceY, 0, wxALL, 5 );
	
	m_staticText91 = new wxStaticText( this, wxID_ANY, wxT("z"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText91->Wrap( -1 );
	fgSizer41->Add( m_staticText91, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	spaceZ = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( spaceZ, 0, wxALL, 5 );
	
	fgSizer3->Add( fgSizer41, 1, wxEXPAND, 5 );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer3->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	fgSizer3->Add( m_sdbSizer1, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer3 );
	this->Layout();
	fgSizer3->Fit( this );
	
	// Connect Events
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( fileInfoDialogUI::OnOK ), NULL, this );
}

fileInfoDialogUI::~fileInfoDialogUI()
{
	// Disconnect Events
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( fileInfoDialogUI::OnOK ), NULL, this );
}

dilateDialogUI::dilateDialogUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, wxT("After dilation adjacent grains with disorientation below this threshold will be merged."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( 320 );
	m_staticText21->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	bSizer7->Add( m_staticText21, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, wxT("Disorientation Threshold (Degrees)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer5->Add( m_staticText13, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	disorientationThreshold = new wxTextCtrl( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( disorientationThreshold, 0, wxALL, 5 );
	
	bSizer7->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer7->Add( m_sdbSizer1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	// Connect Events
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dilateDialogUI::OnOK ), NULL, this );
}

dilateDialogUI::~dilateDialogUI()
{
	// Disconnect Events
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( dilateDialogUI::OnOK ), NULL, this );
}

segmentDialogUI::segmentDialogUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, wxT("Disorientation Threshold (Degrees)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer5->Add( m_staticText13, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	disorientationThreshold = new wxTextCtrl( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( disorientationThreshold, 0, wxALL, 5 );
	
	bSizer7->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer7->Add( m_sdbSizer1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	// Connect Events
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( segmentDialogUI::OnOK ), NULL, this );
}

segmentDialogUI::~segmentDialogUI()
{
	// Disconnect Events
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( segmentDialogUI::OnOK ), NULL, this );
}

filterDialogUI::filterDialogUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, wxT("Grains consisting of fewer voxels than the threshold will be merged into the largest neighbouring grain."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( 340 );
	bSizer7->Add( m_staticText25, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, wxT("Filter Threshold (Voxels)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer5->Add( m_staticText13, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	filterThreshold = new wxTextCtrl( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( filterThreshold, 0, wxALL, 5 );
	
	bSizer7->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer7->Add( m_sdbSizer1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	// Connect Events
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( filterDialogUI::OnOK ), NULL, this );
}

filterDialogUI::~filterDialogUI()
{
	// Disconnect Events
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( filterDialogUI::OnOK ), NULL, this );
}

alignmentFrameUI::alignmentFrameUI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Interpolate Distorted Slices") ), wxHORIZONTAL );
	
	wxArrayString suspiciousListChoices;
	suspiciousList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 150,-1 ), suspiciousListChoices, 0 );
	sbSizer4->Add( suspiciousList, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	sliceWindow = new vtk2DEBSDWindow(this);
	bSizer10->Add( sliceWindow, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	sliceSlider = new wxScrollBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
	bSizer14->Add( sliceSlider, 1, wxALL|wxEXPAND, 5 );
	
	sliceLabel = new wxStaticText( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), wxALIGN_RIGHT );
	sliceLabel->Wrap( -1 );
	sliceLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer14->Add( sliceLabel, 0, wxALL, 5 );
	
	bSizer10->Add( bSizer14, 0, wxEXPAND, 5 );
	
	sbSizer4->Add( bSizer10, 1, wxEXPAND, 5 );
	
	bSizer12->Add( sbSizer4, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer12->Add( m_sdbSizer1, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer12 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( alignmentFrameUI::Init ) );
	suspiciousList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( alignmentFrameUI::SliceSelected ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( alignmentFrameUI::OnCancel ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( alignmentFrameUI::OnOK ), NULL, this );
}

alignmentFrameUI::~alignmentFrameUI()
{
	// Disconnect Events
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( alignmentFrameUI::Init ) );
	suspiciousList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( alignmentFrameUI::SliceSelected ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	sliceSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( alignmentFrameUI::UpdateSlice ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( alignmentFrameUI::OnCancel ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( alignmentFrameUI::OnOK ), NULL, this );
}
