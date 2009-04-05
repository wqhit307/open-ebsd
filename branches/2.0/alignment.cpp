/**	\file 	alignment.cpp
	\brief	Functions for aligning slices and indentifying distorted slices
	
*/

#include "ebsd.h"
#include "prob.H"

/** \def max(a,b)
	\brief return the maximum value of \a a and \a b
*/
/** \def min(a,b)
	\brief return the minimum value of \a a and \a b
*/
#define max(a,b)	((a>b) ? a : b)
#define min(a,b)	((a<b) ? a : b)

#define MAX_SHIFT 30 //!< How many pixels to allow the hillfinding algorithm to shift for the best offset

/// Determine the fraction of coincident voxels in the same grain
inline double Dataset::calculate_ftheta(Point *s1, Point *s2, int dx, int dy) 
{
	double nom=0;
	int x, y, xprime, yprime, d=0;
	Orientation *o1, *o2;
		
	// Cycle through slice  
	//--------------------------------------------------
	for(y=0;y<ty;y++){
		yprime = y+dy;
		for(x=0;x<tx;x++){		
			xprime = x+dx;
			// Check the offset doesn't put outside the slice  
			//--------------------------------------------------
			if ( yprime>0 && yprime<ty && xprime>0 && xprime<tx) {
				o1 = &(s1[x+y*tx].orientation);
				o2 = &(s2[xprime+yprime*tx].orientation);
				if ( o1->is_non_zero() && o2->is_non_zero() ){
					if (o1->disorientation_threshold(o2,QUAT_TOL)) {
						nom = nom+1;
					}
				} else if ( !o1->is_non_zero() && !o2->is_non_zero() ) {
					nom = nom + 0.5;
				}
			} 
			d++;
		} //endfor: sum over y 
	} //endfor: sum over all x

	return nom/d; 
}

/// Initialise the parameters storing information about the slice alignment
int Dataset::init_alignment()
{
	int i;
	
	offsets_calculated = false;
	marked 	   = (bool*) malloc(tz*sizeof(bool));
	suspicious = (bool*) malloc(tz*sizeof(bool));
	offset_x   = (int*)  malloc(tz*sizeof(int));
	offset_y   = (int*)  malloc(tz*sizeof(int));
	probability= (float*)malloc(tz*sizeof(float));
	
	if (suspicious == NULL ||
		marked     == NULL ||
		offset_x   == NULL ||
		offset_y   == NULL) 
			{ return 0; }

	for(i=0;i<tz;i++){
		marked[i] = false;
		suspicious[i] = false;
	}
	
	return 1;
}


/// Calculate (x,y) offsets and attempt to identify suspicious slices
/** Maximises the number of coincident data points with the same orientation.
	Identifies suspicious slices by looking for slices which have an unusually low number
	of coincident pairs with both adjacent slices.
*/
int Dataset::calculate_offsets()
{
	
	int i, y, z, min_x, max_x, min_y, max_y;
	int completed = 0;
	double *cc_z;
	int write_correlation = true;
	int last_unmarked = 0;

	cc_z = (double*) malloc(tz*sizeof(double));
	
	offset_x[0] = 0;
	offset_y[0] = 0;
	min_x = max_x = min_y = max_y = 0;
	
	//Calculate offsets
	#pragma omp parallel for num_threads(4)
	for (z=1;z<tz;z++) {
		
		allow_py_threading;
		double cc_min;
		int i, x, y, yprime, xprime, dx, dy, dx_n, dy_n, dx_o, dy_o;
		Point *s1, *s2;
		
		bool cc_assigned[MAX_SHIFT*2+1][MAX_SHIFT*2+1], finished;
		double cc[MAX_SHIFT*2+1][MAX_SHIFT*2+1];

		char n[100], buff[3]; 
		FILE *fo;

		s1 = data + last_unmarked*dz;
		s2 = data + z*dz;

		if (marked[z]) {
			
			printf("M:%i\n",z);
			offset_x[z] = 0;
			offset_y[z] = 0;
			
		} else if (offsets_calculated && last_unmarked==(z-1)) {
		
			last_unmarked = z;
		
		} else { 

			//-----------------------//
			sprintf(n,"opt/%s-%i.pnm","opt",z);
			if ((fo = fopen(n,"w")) != NULL) {	
				fprintf(fo,"P3\n%i %i 255\n", 2*MAX_SHIFT+1, 2*MAX_SHIFT+1 );
			} else {
				write_correlation = false;
			}
			//-----------------------//
			
			dx_n=0;
			dy_n=0;
			cc_min = -1;
			finished = false;
			
			// Setup array for ftheta  
			//--------------------------------------------------
			for(dx=0;dx<2*MAX_SHIFT+1;dx++){
			for(dy=0;dy<2*MAX_SHIFT+1;dy++){
				cc_assigned[dx][dy] = false;	
			}}
			
			while(!finished){
			
				dx_o = dx_n;
				dy_o = dy_n;
				
				for(dy=max(dy_o-1,-MAX_SHIFT); dy<=min(dy_o+1,MAX_SHIFT); dy++){	
				for(dx=max(dx_o-1,-MAX_SHIFT); dx<=min(dx_o+1,MAX_SHIFT); dx++){
	
	//			for(dy=-MAX_SHIFT; dy<=MAX_SHIFT; dy++){	
	//			for(dx=-MAX_SHIFT; dx<=MAX_SHIFT; dx++){
	
				
					if (!cc_assigned[dx+MAX_SHIFT][dy+MAX_SHIFT]) {
						cc_assigned[dx+MAX_SHIFT][dy+MAX_SHIFT] = true;
						cc[dx+MAX_SHIFT][dy+MAX_SHIFT] = calculate_ftheta(s1, s2, dx, dy);
						if ( cc_min < 0 || cc[dx+MAX_SHIFT][dy+MAX_SHIFT] > cc_min ) {
							finished = false;
							cc_min = cc[dx+MAX_SHIFT][dy+MAX_SHIFT];
							dx_n = dx;
							dy_n = dy;
						}
					}
									
				}}		
				
				if (dx_o == dx_n && dy_n == dy_o) {
					finished = true;
				}
				
			}
			
			cc_z[z] = cc_min;
						
			offset_x[z] = dx_n;
			offset_y[z] = dy_n;
							
			//-------------------//				
			if (write_correlation) {
				for(dx=0;dx<2*MAX_SHIFT+1;dx++){
				for(dy=0;dy<2*MAX_SHIFT+1;dy++){
					if (cc_assigned[dx][dy]) {
						fprintf(fo,"%i %i %i\n",(int)(cc[dx][dy]*255),
												(int)(cc[dx][dy]*255),
												(int)(cc[dx][dy]*255));
					} else {
						fprintf(fo,"255 0 0\n");
					}
				}}
	
				fclose(fo);
			}
			//-------------------//		

		last_unmarked = z;

	}
			
		#pragma flush(completed)		#pragma omp atomic	
		completed++;
		#pragma omp critial(display)
		
		block_py_threading;
		
		progress_callback((float)completed/(tz-1),"Aligning slice", false);
	
	}


	// Look for any suspicious slices  
	//--------------------------------------------------
	
	//Calculate stdev, avg
	double avg_cc=0, stdev_cc=0;
		
	for(z=1;z<tz;z++){	
		avg_cc = avg_cc + cc_z[z];
		stdev_cc = stdev_cc + cc_z[z]*cc_z[z];
	}
	avg_cc = avg_cc/tz;
	stdev_cc = sqrt(stdev_cc/tz - avg_cc*avg_cc);

	printf("avg: %f, stdev: %f\n",avg_cc,stdev_cc);

	double threshold = avg_cc-stdev_cc;
	double p1,p2;
	
	for(z=1;z<tz-1;z++){
		p1 = normal_cdf(cc_z[z],avg_cc,stdev_cc);
		p2 = normal_cdf(cc_z[z+1],avg_cc,stdev_cc);
		if (p1<0.2 && p2<0.2){
			suspicious[z] = true;
			probability[z] = p1*p2*100;
			printf("z:%i (likelihood: %f%%)\n",z,probability[z]);			
		} else {
			suspicious[z] = false;
		}
	}
	
	offsets_calculated = true;
	
	free(cc_z);
	
	return 1;
}

/// Create a new dataset with the slices realigned by the amounts specified in \a offset_x, \a offset_y 
/** 
	Also interpolates slices which have been \a marked 
*/
int Dataset::realign_slices()
{

	int i, y, z, 
		min_x = 0, max_x = 0, 
		min_y = 0, max_y = 0,
		tx_n, ty_n;
	
	double *cc_z;
	
	bool write_correlation = true,
		 completed = false;

	Point *new_data, *n_data, *o_data;
	
	offset_x[0] = 0;
	offset_y[0] = 0;

	// Change coordinates to reference frame
	// As given, they're with respect to the previous slice
	//--------------------------------------------------
	for (z=1;z<tz;z++) {
		if (!marked[z]) {
			offset_x[z] += offset_x[z-1];
			offset_y[z] += offset_y[z-1];
		
			//calculate min/max offsets
			if (offset_x[z] > max_x) {
				max_x = offset_x[z];
			} else if (offset_x[z] < min_x) {
				min_x = offset_x[z];
			}
		
			if (offset_y[z] > max_y) {
				max_y = offset_y[z];
			} else if (offset_y[z] < min_y) {
				min_y = offset_y[z];
			}

		} else {
			offset_x[z] = offset_x[z-1];
			offset_y[z] = offset_y[z-1];
		}
		
	//	printf("%i : %i %i\n",z,offset_x[z],offset_y[z]);
		
	}
	
	// Zero-fill and recentre data
	//--------------------------------------------------
	tx_n = tx + max_x - min_x;
	ty_n = ty + max_y - min_y;
			
	//allocate new data
	printf("%i %i %i %i %i\n",min_x,max_x,tx_n,ty_n,tz);
	new_data = (Point*) malloc(tx_n*ty_n*tz*sizeof(Point));
	if (new_data == NULL) {	return 0; }
	
	n_data = new_data;
	o_data = data;
	
	for(z=0;z<tz;z++){
		// Zero-fill y start
		for(i=0;i<(max_y-offset_y[z])*tx_n;i++) { (n_data++)->set_external(); }
		
		for(y=0;y<ty;y++){
			// Zero-fill x start
			for(i=0;i<(max_x-offset_x[z]);i++) { (n_data++)->set_external(); }			
			
			// Copy the x row
			memcpy(n_data,o_data,tx*sizeof(Point));
			o_data += tx;
			n_data += tx;
			
			// Zero-fill x end
			for(i=0;i<(-min_x+offset_x[z]);i++) { (n_data++)->set_external(); }
		}
	
		// Zero-fill y end
		for(i=0;i<(-min_y+offset_y[z])*tx_n;i++) { (n_data++)->set_external(); }
	
	}

	dims[X] = tx_n;
	dims[Y] = ty_n;

	// Recalculate all geometry dependent variables
	init_geometry();

	// Free up old data and replace with new
	//--------------------------------------------------
	free(data);
	data = new_data;

	// Replace slices that have been marked with interpolated versions
	//-------------------------------------------------- 
	for(z=1;z<tz;z++){
		if (marked[z]){
			replace_slice(z);
		}	
	}

	return 1;	
}


/// Mark a point as outside the dataset 
/**
	These point are created with the dataset is realigned
*/
void Point::set_external()
{
	mad = 0;
	orientation.set_zero();
	bc = 0;
	grain = -1;
	external = true;
}
