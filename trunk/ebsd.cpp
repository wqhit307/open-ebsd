/**
	\file ebsd.cpp
	\brief	Example data processing session 
*/

#include "wxInterface/interface.h"

class MyApp : public wxApp
{
public:
    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();

	Dataset *dataset;
};



IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
	vtkFrame *frame = new vtkFrame(&dataset);    

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}



//int main(int argc,char *argv[])
//{	

	/*

	//Set Data Dimensions
	int dims[3] = {200,200,11};
	float steps[3] = {0.07,0.07,0.101};	

	//Open data file
	Dataset ds = Dataset("../new.txt",dims,steps);
	
	// Calculate offsets
	ds.calculate_offsets();
	
	// Mark slices to replace
	ds.marked[80] = true;
	ds.marked[42] = true;
	
	// Recalculate offsets with replaced slices and realign slices accordingly
	ds.calculate_offsets();	
	ds.realign_slices();

	//Segment, and clean data
	ds.segment(5);
	ds.calculate_grain_info();
	
	ds.dilate_grains();
	ds.calculate_grain_info();

	ds.merge_grains(2);
	ds.calculate_grain_info();

	ds.filter_grains(100);
	ds.calculate_grain_info();
	
	// Calculate ellipsoids, surfaces
	ds.calculate_extra_stats();

	// Write raw file for later processing
	ds.write_raw("raw.ebsd");

	*/
	
//}
