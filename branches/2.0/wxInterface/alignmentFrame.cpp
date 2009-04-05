#include "interface.h"
#ifndef _EBSDH
#include "../ebsd.h"
#endif

alignmentFrame::alignmentFrame( wxWindow* parent, Dataset* dataset ) : alignmentFrameUI( parent, -1 )
{
	ds = dataset;
	
	//sliceWindow = new vtk2DEBSDWindow(this, ds);
	showProgressDialog("Calculating Offsets");
	ds->calculate_offsets();
	clearProgressDialog();
	
	init = false;
	slice = 0;
	
}

void alignmentFrame::OnOK( wxCommandEvent& event )
{
	int tz = ds->dims[Z];
	
	for(int i=0; i<tz; i++){
		ds->marked[i] = suspiciousList->IsChecked(i);
	}
	
	showProgressDialog("Recalculating Offsets");
	ds->calculate_offsets();
	clearProgressDialog();
	showProgressDialog("Aligning Slices");	
	ds->realign_slices();
	clearProgressDialog();

	this->Show(FALSE);

}

void alignmentFrame::OnCancel( wxCommandEvent& event )
{
	this->Show(FALSE);	
}

void alignmentFrame::SliceSelected( wxCommandEvent& event )
{
	int selected = suspiciousList->GetSelection();
	if (selected != wxNOT_FOUND) {
		sliceSlider->SetThumbPosition(selected+1);
		UpdateSlice();
	}
}

void alignmentFrame::UpdateSlice( wxScrollEvent& event )
{
	UpdateSlice();
}

void alignmentFrame::UpdateSlice()
{
	slice = sliceSlider->GetThumbPosition();
	
	sliceWindow->SetSlice(slice);
	
	wxString label;
	label.Printf(wxT("%i"),slice);
	sliceLabel->SetLabel(label);
}


void alignmentFrame::Init( wxActivateEvent& event )
{
	if (!init) {
		int tz = ds->dims[Z];
	
		printf("...%i\n",tz);
	
		wxArrayString suspiciousSlices;
		wxString buff;
	
		for(int i=0; i<tz; i++){	
			if (ds->probability[i] > 1e-6) {
				buff.Printf(wxT("%i (%1.3e%)"),i+1,ds->probability[i]);
			} else {
				buff.Printf(wxT("%i"),i+1);	
			}
			suspiciousSlices.Add( buff );
		}
		suspiciousList->InsertItems(suspiciousSlices,0);	
		
		for(int i=0; i<tz; i++){
			if(ds->suspicious[i]){
				suspiciousList->Check(i);
			}
		}
		
		
		sliceSlider->SetScrollbar(0,1,tz,1);
		sliceWindow->SetDataset(ds);
		
	}
	init = true;
	printf("Activate\n");
}
