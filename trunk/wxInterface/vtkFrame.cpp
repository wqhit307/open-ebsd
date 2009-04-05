#include "interface.h"

vtkFrame::vtkFrame( Dataset **dataset ) : vtkFrameUI( (wxFrame *)NULL, VTK_FRAME, _T("OpenEBSD"), wxPoint(50,50), wxSize(450,340) )
{
	//ConstructVTK();
	//vtkRenderWindow->ConfigureVTK();
	
	grainProp->Show(false);	
	ds = dataset;
}

void vtkFrame::UpdateSlider( wxScrollEvent& event )
{
	UpdateSlider();
}

void vtkFrame::UpdateSlider( bool setCentre )	
{
	if (setCentre) {
		vtkRenderWindow->SetSlices( (*ds)->dims[X]/2, (*ds)->dims[Y]/2, (*ds)->dims[Z]/2 );	
	}
	vtkRenderWindow->SetSlices( xSlider->GetValue(), ySlider->GetValue(), zSlider->GetValue() );		
}

void vtkFrame::SaveRawFile(wxCommandEvent& event)
{

	wxFileDialog dialog(this, _T("Save RAW file as..."),
							  _T(""),_T(""),_T("EBSD Raw files (*.ebsd)|*.ebsd|"),
							  wxFD_SAVE);

	if (dialog.ShowModal() == wxID_OK)
 	{  
		(*ds)->write_raw((const char*)dialog.GetPath().mb_str(wxConvUTF8));
		
	}
	
}

void vtkFrame::progress( float p, char* t )
{
	wxString st;
	st.Printf(wxT("%s"),t);
	progressDialog->Update(p*100,st); 
	Update();	
}

void vtkFrame::UpdateMenuItems()
{
	if ((*ds)->init){
		//sliceAlignmentMenu->enabled = TRUE;
		//saveRawMenu->enabled = TRUE;
	} else {
		//sliceAlignmentMenu->enabled = FALSE;
		//saveRawMenu->enabled = FALSE;
	}
}

void vtkFrame::SliceAlignment( wxCommandEvent& event )
{
	alignmentFrame* alignment = new alignmentFrame(this,*ds);
	alignment->Show(TRUE);	
}

void vtkFrame::OpenCSVFile(wxCommandEvent& event)
{

	wxFileDialog dialog(this, _T("Choose a CSV file to open"),
							  _T(""),_T(""),_T("*.*"),
							  wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dialog.ShowModal() == wxID_OK)
 	{  	
		
		fileInfoDialog infoDialog(this);
		if ( infoDialog.ShowModal() == wxID_OK )
        {
        	showProgressDialog("Processing CSV File");
        									
        	*ds = new Dataset( (const char*)dialog.GetPath().mb_str(wxConvUTF8), 
        					infoDialog.dims, infoDialog.steps, this );   				
        	
        	clearProgressDialog();
        	
        	UpdateSliderLimits();
        	vtkRenderWindow->SetDataset(*ds);  
        	UpdateMenuItems();
        }
			
	}
	
}

void vtkFrame::OpenRawFile(wxCommandEvent& event)
{

	wxFileDialog dialog(this, _T("Choose a RAW file to open"),
							  _T(""),_T(""),_T("EBSD Raw files (*.ebsd)|*.ebsd|"),
							  wxFD_OPEN || wxFD_FILE_MUST_EXIST);

	if (dialog.ShowModal() == wxID_OK)
 	{  
		*ds = new Dataset( (const char*)dialog.GetPath().mb_str(wxConvUTF8), this );
		UpdateSliderLimits();
		
		vtkRenderWindow->SetDataset(*ds);
		UpdateMenuItems();
	}
	
}

void vtkFrame::UpdateSliderLimits()
{
	if ((*ds)->init) {
		xSlider->SetRange(1,(*ds)->dims[X]);	
		ySlider->SetRange(1,(*ds)->dims[Y]);	
		zSlider->SetRange(1,(*ds)->dims[Z]);	

	}
	
	UpdateSlider( true );
	vtkRenderWindow->SetDataset(*ds);
	
}

void vtkFrame::Segment( wxCommandEvent& event )
{
	segmentDialog dialog(this);
	if ( dialog.ShowModal() == wxID_OK ) {
		
        showProgressDialog("Segmenting Dataset");        									
       	(*ds)->segment(dialog.threshold);
       	(*ds)->calculate_grain_info();   				
       	clearProgressDialog();
       	vtkRenderWindow->RefreshData();
        	
	}
}

void vtkFrame::Dilate( wxCommandEvent& event )
{
	dilateDialog dialog(this);
	if ( dialog.ShowModal() == wxID_OK ) {
		
        showProgressDialog("Dilating Dataset");        									
       	(*ds)->dilate_grains();
       	(*ds)->calculate_grain_info();
       	(*ds)->merge_grains(dialog.threshold);
       	(*ds)->calculate_grain_info();   				
       	clearProgressDialog();
       	vtkRenderWindow->RefreshData();
        	
	}
}

void vtkFrame::Filter( wxCommandEvent& event )
{
	filterDialog dialog(this);
	if ( dialog.ShowModal() == wxID_OK ) {
		
        showProgressDialog("Filtering Dataset");        									
       	(*ds)->filter_grains(dialog.threshold);
       	(*ds)->calculate_grain_info();
       	clearProgressDialog();
       	vtkRenderWindow->RefreshData();
        	
	}
}

void vtkFrame::CalculateExtraStats(  wxCommandEvent& event )
{
	showProgressDialog("Filtering Dataset");        									
    (*ds)->calculate_extra_stats();
    clearProgressDialog();	
}

