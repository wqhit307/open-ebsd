#include "interface.h"

// File Info Dialog
//---------------------------------------------
fileInfoDialog::fileInfoDialog( wxWindow* parent ) : fileInfoDialogUI( parent )
{}

void fileInfoDialog::InitDialog()
{
	m_dims[X] = _T("200");
	m_dims[Y] = _T("200");
	m_dims[Z] = _T("30");
	
	m_steps[X] = _T("1");
	m_steps[Y] = _T("1");
	m_steps[Z] = _T("1");

	// Set validators
	dimX->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_dims[X]));
	dimY->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_dims[Y]));
	dimZ->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_dims[Z]));

	spaceX->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_steps[X]));
	spaceY->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_steps[Y]));
	spaceZ->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_steps[Z]));

	wxDialog::InitDialog();
}

void fileInfoDialog::OnOK(wxCommandEvent& event)
{
int i;
bool valuesOK = true;

if ( Validate() && TransferDataFromWindow() )
    {
    
    	for(i=0;i<3;i++){
    		dims[i]  = atoi( m_dims[i].mb_str(wxConvUTF8) );
    		steps[i] = atol( m_steps[i].mb_str(wxConvUTF8) );
    		
    		if ( dims[i]  <= 0 ) { valuesOK = false; }
    		if ( steps[i] <= 0 ) { valuesOK = false; }

    	}
    	
    	if ( valuesOK ) {
    
	        if ( IsModal() ) {
	            EndModal(wxID_OK);
    	    } else {
    	        SetReturnCode(wxID_OK);
    	        this->Show(false);
    	    }
    	
    	} else {
    	
    		wxMessageBox(_T("All values must be positive"));
    	
    	}
    }
}

// Segment Dialog
//---------------------------------------------
segmentDialog::segmentDialog( wxWindow* parent ) : segmentDialogUI( parent )
{}

void segmentDialog::InitDialog()
{
	m_threshold = wxT("3");
	
	// Set validators
	disorientationThreshold->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_threshold));
	
	wxDialog::InitDialog();
}

void segmentDialog::OnOK( wxCommandEvent& event )
{
bool valuesOK = true;

if ( Validate() && TransferDataFromWindow() )
    {
    
    	threshold = atof( m_threshold.mb_str(wxConvUTF8) );
    	if ( threshold <= 0 ) { valuesOK = false; }	
   	
    	if ( valuesOK ) {
    
	        if ( IsModal() ) {
	            EndModal(wxID_OK);
    	    } else {
    	        SetReturnCode(wxID_OK);
    	        this->Show(false);
    	    }
    	
    	} else {
    	
    		wxMessageBox(_T("Threshold must be positive"));
    	
    	}
    }
}

// Filter Dialog
//---------------------------------------------
filterDialog::filterDialog( wxWindow* parent ) : filterDialogUI( parent )
{}

void filterDialog::InitDialog()
{
	m_threshold = wxT("100");
	
	// Set validators
	filterThreshold->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_threshold));
	
	wxDialog::InitDialog();
}

void filterDialog::OnOK( wxCommandEvent& event )
{
bool valuesOK = true;

if ( Validate() && TransferDataFromWindow() )
    {
    
    	threshold = atoi( m_threshold.mb_str(wxConvUTF8) );
    	if ( threshold <= 0 ) { valuesOK = false; }	
   	
    	if ( valuesOK ) {
    
	        if ( IsModal() ) {
	            EndModal(wxID_OK);
    	    } else {
    	        SetReturnCode(wxID_OK);
    	        this->Show(false);
    	    }
    	
    	} else {
    	
    		wxMessageBox(_T("Threshold must be positive"));
    	
    	}
    }
}

// Dilate Dialog
//---------------------------------------------
dilateDialog::dilateDialog( wxWindow* parent ) : dilateDialogUI( parent )
{}

void dilateDialog::InitDialog()
{
	m_threshold = wxT("3");
	
	// Set validators
	disorientationThreshold->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_threshold));
	
	wxDialog::InitDialog();
}

void dilateDialog::OnOK( wxCommandEvent& event )
{
bool valuesOK = true;

if ( Validate() && TransferDataFromWindow() )
    {
    
    	threshold = atof( m_threshold.mb_str(wxConvUTF8) );
    	if ( threshold <= 0 ) { valuesOK = false; }	
   	
    	if ( valuesOK ) {
    
	        if ( IsModal() ) {
	            EndModal(wxID_OK);
    	    } else {
    	        SetReturnCode(wxID_OK);
    	        this->Show(false);
    	    }
    	
    	} else {
    	
    		wxMessageBox(_T("Threshold must be positive"));
    	
    	}
    }
}
