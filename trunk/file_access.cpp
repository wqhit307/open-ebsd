/**	\file 	file_access.cpp
	\brief	Handles all file input/output
	
	Contains functions for importing Channel5 CSV files, reading and writing raw files
	and image output
*/


#include "ebsd.h"

#define LINE_BUFFER_SIZE	1000		//!< Size of line input buffer for reading Channel5 CSV
#define MAGIC				"EBSD RAW"	//!< 'Magic' string to identify EBSD raw files

/// Display progress of process \a label on command line
void disp_progress(const char *label, float pc)
{
	char buff[1000];

	printf("%s] %i%%",buff, (int)(pc*100));
	printf("\r%s: %i%%",label, (int)(pc*100));
	
	if (pc==1) { printf("\n"); }

}

/// Import a Channel 5 CSV file
/**
	Expects a rows for each data point seperated by semicolons,
	in order of increasing z,y,x with x cycling fastest.
*/
int Dataset::read_channel5(const char name[])
{
	FILE *fi;
	char line[LINE_BUFFER_SIZE];
	char *buff = NULL;
	EULER e;
	
	int i = 0, j = 0, k = 0; 
	int pf = nv/50;
	 	
	Point *c;
	
	if ((fi = fopen(name,"r")) == NULL) {
		return 0;
	} else {
					
		//Ignore header line
		fgets(line, LINE_BUFFER_SIZE, fi);
	
		data = (Point*) malloc(sizeof(Point)*nv);	
		if (data == NULL) { return 0; }
		
		c = data;
		//Cycle through values
		while ( i++ < nv ) {
			allow_py_threading;
			if (fgets(line, LINE_BUFFER_SIZE, fi) == NULL ) { break; }
			block_py_threading;
			
			buff = strtok(line,";");
			for (j=EULER_COL;j--;) 
				buff = strtok(NULL,";");
			
			e[0] 		= strtod(buff,&buff);
			e[1] 		= strtod(buff+3,&buff);
			e[2] 		= strtod(buff+3,&buff);			
			c->mad      = strtod(buff+3,&buff);
			c->bc       = strtod(buff+3,&buff);
			c->grain    = -1;
			c->external = false;

			c->orientation = Orientation(e[0],e[1],e[2]);
			c->orientation.reduce_zone();
			
			c++;
			
			if (i % pf == 0) {
				//progress_callback((float)i/nv, "Reading file", false);
				callback->progress((float)i/nv);
			}
			
		}
			
		fclose(fi);
		return 1;
		
	}
}

/// Read raw EBSD file
/**
	Reads a dataset exported from the system.	
*/
int Dataset::read_raw(const char name[])
{
	FILE *fi;
	int i;
	char buff[100];
	if (name !=NULL && (fi = fopen(name,"rb")) != NULL) {

		fgets(buff,100,fi);

		if (strncmp(buff,MAGIC,8) == 0) {	
			
			fread(dims, sizeof(int),3,fi);
			fread(steps,sizeof(float),3,fi);
			fread(&num_grains,sizeof(int),1,fi);
	
			nv = dims[0]*dims[1]*dims[2];
			data = (Point*) malloc(sizeof(Point)*nv);
			if (data == NULL) { return 0; }
										
			fread(data,sizeof(Point),nv,fi);
								
			fclose(fi);
			return 1;
		
		} else {
			fclose(fi);
			return 0;
		}
	}
	
	return 1;
}

/// Dump the raw point data from memory to a file
/**
	Only guaranteed to be readable on the same system that wrote it. 
	Useful for saving a partially processed dataset between sessions.
	
	We only write the point data and number of grains - the rest can be
	fairly easily reconstructed from this.
*/ 
int Dataset::write_raw(const char name[])
{
	FILE *fo;
	int i;
	
	if (name!=NULL && (fo = fopen(name,"wb")) != NULL) {
		fprintf(fo,"%s\n",MAGIC);
		
		fwrite(dims,sizeof(int),3,fo);
		fwrite(steps,sizeof(float),3,fo);
		fwrite(&num_grains,sizeof(int),1,fo);
		fwrite(data,sizeof(Point),nv,fo);
		
		printf(">>>%i\n",num_grains);
		
		fclose(fo);
		return 1;
	} else {
		return 0;
	}
}

/// Export grain data to a set of CSV files for processing in another application
int Dataset::write_csv(const char name[])
{
	FILE *fo;
	int i,j;
	
	float *e;
	float dV = steps[X]*steps[Y]*steps[Z];
	
	if (name!=NULL && (fo = fopen("ex-1.csv","w")) != NULL) {
		fprintf(fo,"Grain Number,Edge,Points,Volume,Surface Area,Adjacent Grains,phi1,PHI,phi2,a,b,c,ba,ca,c_x,c_y,c_z\n");
		
		for(i=0;i<num_grains;i++){		
				
			e = grains[i].orientation.euler();
		
			fprintf(fo,"%i,%i,%i,%f,%f,%i,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
					  i,
					  grains[i].on_edge,
					  grains[i].volume,
					  grains[i].volume*dV,
					  grains[i].surface_area,
					  grains[i].num_adj_grains,
					  
					  e[phi1], e[PHI], e[phi2],
					  grains[i].axis[0],
					  grains[i].axis[1],
					  grains[i].axis[2],
					  grains[i].axis[1]/grains[i].axis[0],
					  grains[i].axis[2]/grains[i].axis[0],
					  grains[i].centroid[1],
					  grains[i].centroid[2],
					  grains[i].centroid[3]);

		}
		
		fclose(fo);
	} else {
		return 0;
	}
		
	if (name!=NULL && (fo = fopen("ex-2.csv","w")) != NULL) {
		for(i=0;i<num_grains;i++){
		for(j=0;j<grains[i].num_adj_grains;j++){	
								
			fprintf(fo,"%f\n",
					  grains[i].adj_grains[j].disorientation);
		}}
		
		fclose(fo);
	} else {
		return 0;
	}

	int t,d;
	float f;
	
	if (name!=NULL && (fo = fopen("ex-3.csv","w")) != NULL) {
		for(i=0;i<num_grains;i++){
		
			t = grains[i].num_adj_grains;
			d=0;
			
			for(j=0;j<grains[i].num_adj_grains;j++){	
									
				if (grains[i].adj_grains[j].disorientation < 15) {
					d++;
				}
			}
		
			f = (float)d / (float)t;
			fprintf(fo,"%f\n",f);		
		}
		
		fclose(fo);
	} else {
		return 0;
	}


	
	return 1;
}

/// Write a collection of PPM image files, one for each slice
/**
	Several modes can be specified:
		MODE_EULER - map euler angles to RGB
		MODE_AVG_EULER - map the grain average euler angles to RGB
		MODE_RF - map RF vector to RGB
		MODE_EULER - map the grain average RF vector to RGB
		MODE_GRAIN - map the grain number to greyscale
		
*/
int Dataset::write_ppm(const char name[], int mode) {

	FILE *fo;
	char n[100];
	unsigned char buff[3];
	Point *d;
	int i, z;
	char c[1];
	float *e, *r;

	float e_external[3] = {10,10,10};
	float e_undef[3] = {255,0,0};
	float r_undef[3] = {1,0,0};
	
	c[0] = (unsigned char)10;
		
	d = data;
	
	for(z=0;z<tz;z++) {
		sprintf(n,"%s-%i.pnm",name,z);
		
		if ((fo = fopen(n,"w")) != NULL) {
			
			if (mode==MODE_GRAIN || mode == MODE_MAD) {
				fprintf(fo,"P2\n%i %i 65535\n", dims[0], dims[1],c);
			} else if (mode==MODE_EULER || mode==MODE_AVG_EULER || mode==MODE_RF || mode==MODE_AVG_RF) {			
				fprintf(fo,"P3\n%i %i 255\n", dims[0], dims[1],c);
			}
			for(i=0;i<dz;i++) {				
				if (mode==MODE_GRAIN){
					fprintf(fo,"%i ", (d->grain));
					if (i%40==0){ fprintf(fo,"\n"); }
					
				} else if (mode==MODE_MAD){
					fprintf(fo,"%i ", (d->mad)*30000);
					if (i%40==0){ fprintf(fo,"\n"); }
					
				} else if (mode==MODE_AVG_EULER) {
					if(d->external==1) {
						e = e_external;
					} else if(d->grain==-1) {
						e = e_undef;
					} else {
						e = grains[d->grain].orientation.euler();
					}
					fprintf(fo,"%i %i %i\n", (int)(e[0]),
											 (int)(e[1]),
											 (int)(e[2]));
											 
				} else if (mode==MODE_AVG_RF) {
					if (d->grain>-1) {
						r = grains[d->grain].orientation.rf();
					} else {
						r = r_undef;
					}
					fprintf(fo,"%i %i %i\n", (int)(fabs(r[0])*255),
											 (int)(fabs(r[1])*255),
											 (int)(fabs(r[2])*255));
											 
				} else if (mode==MODE_EULER) {
					e = d->orientation.euler();
					fprintf(fo,"%i %i %i\n", (int)(e[0]),
											 (int)(e[1]),
											 (int)(e[2]));
											 
				} else if (mode==MODE_RF) {
					if (d->orientation.is_non_zero()){
						r = d->orientation.rf();
						fprintf(fo,"%i %i %i\n", (int)(fabs(r[0])*255),
												 (int)(fabs(r[1])*255),
												 (int)(fabs(r[2])*255));
					} else {
						fprintf(fo,"%i %i %i\n", 10,10,10);

					}
				}
				
				d++;
				
			}
			
			fclose(fo);

		} else {
			return 0;
		}
	}
	
	return 1;
}


/// Export the dataset to an Amira Mesh file 
int Dataset::write_am(const char name[]) {

	FILE *fo;
	int i = 0; 
	float *e;
	
	if ((fo = fopen(name,"w")) == NULL) {
		return 0;
	} else {
		//Write header for file
	
		float *coords;
		coords = (float*)malloc(4*dims[2]);
		for (i=0;i<tz;i++) {
			*(coords+i) = steps[2]*i;
		}
	
		fprintf(fo,"# AmiraMesh ASCII 1.0\n\n");
		fprintf(fo,"define Lattice %i %i %i\n",dims[0],dims[1],dims[2]);
		fprintf(fo,"define Coordinates %i\n", dims[2]);
		fprintf(fo,"Parameters {\n	CoordType \"stacked\",\n");
		fprintf(fo,"	BoundingBoxXY 0 %f 0 %f\n}\n\n", dims[0]*steps[0], dims[1]*steps[1]);
		fprintf(fo,"Lattice { float[3] Intensity } = @2\n");
		fprintf(fo,"Coordinates { float z } = @1\n\n@1\n");
		
		for(i=0;i<tz;i++){
			fprintf(fo,"%f ",*(coords+i));
		}

		fprintf(fo,"\n@2\n");

		//fwrite(coords,sizeof(float),dims[2],fo);
		//fwrite(data,sizeof(float),nv*d,fo);

		for(i=0;i<nv;i++){
			e = (data+i)->orientation.euler();
			fprintf(fo,"%f %f %f ",e[0],e[1],e[2]);
			if (i % 100 == 0)
				fprintf(fo,"\n");
		}
		fclose(fo);
		
		free(coords);
		
		return 1;
	}
}

void Point::map_rgb(int mode, int* rgb, Grain* grains)
{		
	float *r;
	float r_external[3] = {10,10,10};
	float r_undef[3] = {255,0,0};
	
	
	if (mode==MODE_AVG_EULER) {
		if( external==1 ) {
			r = r_external;
		} else if(grain==-1) {
			r = r_undef;
		} else {
			r = grains[grain].orientation.euler();
		}
								 
	} else if (mode==MODE_AVG_RF) {
		if (grain>-1) {
			r = grains[grain].orientation.rf();
			r[R] = r[R]*255;
			r[G] = r[G]*255;
			r[B] = r[B]*255;
		} else {
			r = r_undef;
		}
								 
	} else if (mode==MODE_EULER) {
		r = orientation.euler();
								 
	} else if (mode==MODE_RF) {
		r = orientation.rf();
		r[R] = r[R]*255;
		r[G] = r[G]*255;
		r[B] = r[B]*255;
	}
	
	rgb[R] = (int) r[R];
	rgb[G] = (int) r[G];
	rgb[B] = (int) r[B];
}
