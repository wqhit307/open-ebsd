/**
	\file symmetry.h
	\brief Declarations used for symmetry calculations
*/

#ifndef _SYMMETRYH_
#define _SYMMETRYH_

#define SYM_NUM 24
#define PI		3.14159265358979323846264338327950288419716939937510

#define phi1	0
#define PHI		1
#define phi2	2

static float ist2 = 1/(float)sqrt((float)2);

static float DEG = (float)PI/180;
static float RAD = (float)180/PI;

static float QUAT_TOL = cos(EULER_TOL*0.5*DEG);
static float TEN_DEG = cos(10*0.5*DEG);

#define quat_non_zero(q) (q[0] != 0 || q[1] != 0 || q[2] != 0 || q[3] != 1)

static QUAT Sq[24] = {

	{0,0,0,1},
	
	{1,0,0,0},
	{0,1,0,0},
	{0,0,1,0},
	
	{0.5,0.5,0.5,0.5},
	{-0.5,0.5,0.5,0.5},
	{0.5,-0.5,0.5,0.5},
	{0.5,0.5,-0.5,0.5},

	{-0.5,-0.5,0.5,0.5},
	{-0.5,0.5,-0.5,0.5},
	{0.5,-0.5,-0.5,0.5},
	{-0.5,-0.5,-0.5,0.5},
	
	{ist2,0,0,ist2},
	{-ist2,0,0,ist2},

	{0,ist2,0,ist2},
	{0,-ist2,0,ist2},

	{0,0,ist2,ist2},
	{0,0,-ist2,ist2},
	
	{ist2,ist2,0,0},
	{-ist2,ist2,0,0},

	{0,ist2,ist2,0},
	{0,ist2,-ist2,0},

	{ist2,0,ist2,0},
	{-ist2,0,ist2,0},
	
};

#endif