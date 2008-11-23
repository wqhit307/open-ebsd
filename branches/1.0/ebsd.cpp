/**
	\file ebsd.cpp
	\brief	Example data processing session 
*/

#include "ebsd.h"

int main(int argc,char *argv[])
{	

	//Set Data Dimensions
	int dims[3] = {200,200,116};
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
	
}
