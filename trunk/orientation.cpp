/**
	\file orientation.cpp
	\brief Methods for the Orientation class
*/

#include "ebsd.h"

/// Initialise Orientation with quaternion		
Orientation::Orientation(float x, float y, float z, float w)
{
	q[X] = x;
	q[Y] = y;
	q[Z] = z;
	q[W] = w;
	euler_set = false;
}

/// Initialise Orientation with quaternion
Orientation::Orientation(float *qR)
{		
	set_q(qR);
}

/// Initialise Orientation with euler angles
/**
	Requires zxz Bunge-Euler angles in rotating frame  
*/
Orientation::Orientation(float e_phi1, float e_PHI, float e_phi2)
{
	float sx = sin( e_PHI*DEG*0.5 );
	float cx = cos( e_PHI*DEG*0.5 );
		
	q[0] = sx*cos( (e_phi1 - e_phi2)*0.5*DEG );
	q[1] = sx*sin( (e_phi1 - e_phi2)*0.5*DEG );
	q[2] = cx*sin( (e_phi1 + e_phi2)*0.5*DEG );
	q[3] = cx*cos( (e_phi1 + e_phi2)*0.5*DEG );
	
	if (q[W]<0) {
		q[X] = -q[X];
		q[Y] = -q[Y];
		q[Z] = -q[Z];
		q[W] = -q[W];
	}

	e[phi1] = e_phi1;
	e[PHI ] = e_PHI;
	e[phi2] = e_phi2;
	
	euler_set = true;

}

/// Initialise Orientation with zero quaternion
Orientation::Orientation()
{
	set_zero();
}

/// Set the value of the quaternion
void Orientation::set_q(const float *qR)
{		
	q[X] = qR[X];
	q[Y] = qR[Y];
	q[Z] = qR[Z];
	q[W] = qR[W];
	euler_set = false;
}

/// Set the value of the quaternion
void Orientation::set_q(const long double *qR)
{		
	q[X] = (float)qR[X];
	q[Y] = (float)qR[Y];
	q[Z] = (float)qR[Z];
	q[W] = (float)qR[W];
	euler_set = false;
	
}


/// Inverts the rotation represented by the Orientation
void Orientation::invert()
{
	q[X] = -q[X];
	q[Y] = -q[Y];
	q[Z] = -q[Z];
}

/// Set the Orienation to a zero rotation
void Orientation::set_zero()
{
	q[X] = 0;
	q[Y] = 0;
	q[Z] = 0;
	q[W] = 1;
	euler_set = false;
}

/// Check if Orientation is non-zero
bool Orientation::is_non_zero()
{
	return !(q[X]==0 && q[Y]==0 && q[Z]==0 && q[W]==1);
}


/// Combine two rotations
/**
	Returns result as float array \a qR 
*/
void Orientation::combine(const float *qB, float *qR)
{
	qR[X] = qB[X]*q[W] + qB[W]*q[X] - qB[Y]*q[Z] + qB[Z]*q[Y];
	qR[Y] = qB[Y]*q[W] + qB[W]*q[Y] - qB[Z]*q[X] + qB[X]*q[Z];
	qR[Z] = qB[Z]*q[W] + qB[W]*q[Z] - qB[X]*q[Y] + qB[Y]*q[X];
	qR[W] = qB[W]*q[W] - qB[X]*q[X] - qB[Y]*q[Y] - qB[Z]*q[Z];
	
	if (qR[W]<0) {
		qR[X] = -qR[X];
		qR[Y] = -qR[Y];
		qR[Z] = -qR[Z];
		qR[W] = -qR[W];
	}		
}

/// Calculate the w component of the quaternion obtained by combining this Orientation and that represented by qR
inline float Orientation::combine_W(const float* qR)
{
	return fabs(q[W]*qR[W] - q[X]*qR[X] - q[Y]*qR[Y] - q[Z]*qR[Z]);
}

/// Force the quaternion into the fundamental zone
void Orientation::reduce_zone()
{
	int i,im = -1;
	float dq3, mdq3=0;
	float qR[4];

	Orientation ox = q;
	
	for(i=0;i<SYM_NUM;i++){
		dq3 = combine_W(Sq[i]);
		if (dq3>mdq3){
			im = i;
			mdq3=dq3;
		}
	}

	ox.combine(Sq[im],qR);
	set_q(qR);
	
}

/// Set the quaternion such that the misorientation with Orientation o is minimised
void Orientation::reduce_zone(Orientation *o)
{
	int i, im = 0;
	float dq3, mdq3 = 0;
	float qR[4];
	
	Orientation oR = *o;
	oR.invert();

	for(i=0;i<SYM_NUM;i++){
		combine(Sq[i],qR);
		dq3 = oR.combine_W(qR);
		if (dq3>mdq3){
			im = i;
			mdq3=dq3;
		}
	}


	if (im>0){
		combine(Sq[im],qR);
		set_q(qR);
	}
}

/// Determine if the disorientation between this orientation and Orientation o is below a certain threshold
/**
	@param threshold disorientation threshold, in the form $cos(\frac{\theta}{2}) for performance reasons
*/
bool Orientation::disorientation_threshold(Orientation *o, float threshold)
{
	int i;
	float dq3, m = 0;
	float qR[4];
	Orientation oR = q;
	oR.invert();

	for(i=0;i<SYM_NUM;i++){
		o->combine(Sq[i],qR);
		dq3 = oR.combine_W(qR);

		if (dq3 > m) {
			m = dq3;
		}

		if (dq3>=threshold) {
			return true;			
		}
	}

	return false;	
	
}


/// Calculate the smallest quaternion relating this orientation to Orientation o
Orientation Orientation::misorientation(Orientation *o)
{
	int i,im;
	float dq3, mdq3 = 0;
	float qR[4],qS[4];
	
	Orientation oR = *this;
	oR.invert();

	for(i=0;i<SYM_NUM;i++){
		oR.combine(Sq[i],qR);
		dq3 = o->combine_W(qR);
		
		if (dq3>mdq3){
			mdq3 = dq3;
			im=i;
			if (dq3>TEN_DEG) { break; }
		}
	}
	
	oR.combine(Sq[im],qR);
	o->combine(qR,qS);
	
	return Orientation(qS);
	
}

/// Calculate the disorientation between this orientation and Orientation \a o
float Orientation::disorientation(Orientation *o)
{
	int i,im;
	float dq3, mdq3 = 0, d;
	float qR[4];
	
	Orientation oR = q;
	oR.invert();

	for(i=0;i<SYM_NUM;i++){
		o->combine(Sq[i],qR);
		dq3 = oR.combine_W(qR);
		if (dq3>mdq3){
			mdq3 = dq3;
			im=i;
		}
	}

	if (1-dq3 > 16*FLT_EPSILON) { d = 2*acos(mdq3)*RAD; } 
	else 						{ d = 0; }
	
	return d;	

}


/// Combine two rotations and return result as new Orientation
Orientation Orientation::operator * (const Orientation o)
{
	float q[4];
	combine(o.q,q);
	return Orientation(q);
}

/// Set the orientation
Orientation Orientation::operator = (const Orientation o)
{
	set_q(o.q);
	return *this;
}

/// Set the orientation
Orientation Orientation::operator = (const long double* qA)
{
	set_q(qA);
	return *this;
}

/// Set the orientation
Orientation Orientation::operator = (const float* qA)
{
	set_q(qA);
	return *this;
}



/// Calculate and return euler angles for Orientation
/**
	The first time this function is called after the Orientation has changed the Euler 
	angles will be calculated and stored, subequent calls will return the stored value. 
*/
float* Orientation::euler()
{		
	if (!euler_set) {
		double M[4][4];

		double Nq = q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
		double s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;
		double xs = q[0]*s,	  	  ys = q[1]*s,	 zs = q[2]*s;
		double wx = q[3]*xs,	  wy = q[3]*ys,	 wz = q[3]*zs;
		double xx = q[0]*xs,	  xy = q[0]*ys,	 xz = q[0]*zs;
		double yy = q[1]*ys,	  yz = q[1]*zs,	 zz = q[2]*zs;
		
		M[X][X] = 1.0 - (yy + zz); M[X][Y] = xy - wz; M[X][Z] = xz + wy;
		M[Y][X] = xy + wz; M[Y][Y] = 1.0 - (xx + zz); M[Y][Z] = yz - wx;
		M[Z][X] = xz - wy; M[Z][Y] = yz + wx; M[Z][Z] = 1.0 - (xx + yy);
		M[W][X]=M[W][Y]=M[W][Z]=M[X][W]=M[Y][W]=M[Z][W]=0.0; M[W][W]=1.0;
	
		double sy = sqrt(M[0][2]*M[0][2] + M[0][1]*M[0][1]);
		if (sy > 16*FLT_EPSILON) {
			e[phi1] = (float)atan2(M[2][0], -M[1][0])*RAD;
			e[PHI]  = (float)atan2(sy, M[0][0])*RAD;
			e[phi2] = (float)atan2(M[0][2], M[0][1])*RAD;
		} else {
			e[phi1] = 0;
			e[PHI]  = (float)atan2(sy, M[0][0])*RAD;
			e[phi2] = (float)atan2(-M[2][1], M[2][2])*RAD;
		}
		
		if (e[phi1] < 0) { e[phi1] = 360+e[phi1]; }
		if (e[PHI] < 0)  { e[PHI]  = 360+e[PHI ]; }
		if (e[phi2] < 0) { e[phi2] = 360+e[phi2]; }
		
		euler_set = true;
	}
	
	return e;
}

/// Return the Rodrigues vector representing the orientation
float* Orientation::rf()
{		
	r[0] = q[X]/q[W] + 0.5;
	r[1] = q[Y]/q[W] + 0.5;
	r[2] = q[Z]/q[W] + 0.5;
	
	return r;
}

