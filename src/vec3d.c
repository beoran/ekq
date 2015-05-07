
/* 3D Vector types implementation. */

#include <math.h>
#include "vec3d.h"


Vec3d vec3d(double x, double y, double z) { 
  Vec3d result;
  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}

Vec3d vec3d_0() { 
  return vec3d(0.0, 0.0, 0.0);
}

Vec3d vec3d_add(Vec3d v1, Vec3d v2) { 
  return vec3d(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vec3d vec3d_sub(Vec3d v1, Vec3d v2) { 
  return vec3d(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vec3d vec3d_mul(Vec3d v1, double factor) { 
  return vec3d(v1.x * factor, v1.y * factor, v1.z * factor );
}

Vec3d vec3d_neg(Vec3d v1) { 
  return vec3d(-v1.x, -v1.y, -v1.z );
}

/** 3D vector dot product. */
double vec3d_dot(Vec3d v1, Vec3d v2) { 
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}


Vec3d vec3d_div_unsafe(Vec3d v1, double factor) { 
  return vec3d(v1.x / factor, v1.y / factor, v1.z / factor);
}

Vec3d vec3d_div(Vec3d v1, double factor) { 
  if (factor == 0.0) {
    return vec3d_0();
  }
  return vec3d_div_unsafe(v1, factor);
}


double vec3d_lengthsq(Vec3d v1) { 
  return (v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z); 
}

double vec3d_length(Vec3d v1) { 
  return sqrt(vec3d_lengthsq(v1)); 
}

Vec3d vec3d_normalize(Vec3d v1) {
  double length = vec3d_length(v1);
  return vec3d_div(v1, length);
}

Vec3d vec3d_normalize_unsafe(Vec3d v1) {
  double length = vec3d_length(v1);
  return vec3d_div_unsafe(v1, length);
}

Vec3d vec3d_project(Vec3d vec , Vec3d on) {
  double dot            = vec3d_dot(vec, on);
  double lengthsq       = vec3d_lengthsq(on);
  double x              = on.x * dot / lengthsq;
  double y              = on.y * dot / lengthsq;
  double z              = on.z * dot / lengthsq;
  
  return vec3d(x, y, z);
}

/** XXX: check these ones or 3D */
Vec3d vec3d_rightnormal(Vec3d vec) {
  return vec3d(-vec.y, vec.x, vec.z);
}

Vec3d vec3d_leftnormal(Vec3d vec) {
  return vec3d(vec.y, -vec.x, vec.z);
}

double vec3d_perproduct(Vec3d v1, Vec3d v2) {
  return vec3d_dot(v1, vec3d_rightnormal(v2));  
}

Vec3d vec3d_forangle(double angle) {
  return vec3d(cos(angle), sin(angle), 0.0);
}

/* Returns a unit vector that makes the given angle a with the X axis. */
Vec3d vec3d_for_angle(double a) {       
  return vec3d(cos(a), sin(a), 0.0);
}

double vec3d_toangle(Vec3d v1) {
  return atan2(v1.y, v1.x);
}

/* Cross product magnitude. The cross product of 2D x,y vectors is
   a 3D vector with a pure z component, so this function returns only the     
  magnitude of that z component. XXX: fix this for 3D! */
Vec3d vec3d_cross(Vec3d self, Vec3d other) {
  return vec3d(self.y * other.z - self.z * other.y, 
               self.z * other.x - self.x * other.z,
               self.x * other.y - self.y * other.x
              );
}


/* Perpendicular vector, rotated by 90 degrees, anticlockwise. */
Vec3d vec3d_perp(Vec3d self) { 
  return vec3d(-self.y, self.x, self.z);
}

/* Perpendicular vector, rotated by -90 degrees, clockwise. */
Vec3d vec3d_nperp(Vec3d self) { 
  return vec3d(self.y, -self.x, self.z);
}
 
 /* Returns a vector that is the projection of self onto other. 
  * Not possible if other has a length 0. */
Vec3d vec3d_project_2(Vec3d self, Vec3d other) {
  double dot_product, other_length;
  dot_product   = vec3d_dot(self, other);
  other_length  = vec3d_length(other);
  return vec3d_mul(other, dot_product  / other_length);
}

      
/* Rotates self by other. XXX fix this! */
Vec3d vec3d_rotate(Vec3d self, Vec3d other) {
  return vec3d(self.x*other.x - self.y*other.y, 
               self.x*other.y + self.y*other.x, 
               0.0);
}
 
/* Inverse rotation. XXX fix this! */
Vec3d vec3d_unrotate(Vec3d self, Vec3d other) {
  return vec3d(self.x*other.x + self.y*other.y, 
               self.x*other.y - self.y*other.x, 
               0.0);
}
 
/* Linear interpolation on a line between between self and other. 
 * Returns a vector that points to a point on the line between self and other. */
Vec3d vec3d_lerp(Vec3d self, Vec3d other, double tx) { 
  return vec3d_add(vec3d_mul(self, 1.0 - tx), vec3d_mul(other, tx)); 
  /* self * (1.0 - tx) + (other * tx) */
}
    
/* If the length of the vector is greater than max, this function 
    returns a vector with the same direction as self, but with the length 
    of that vector limited to max. Otherwise, it returns a copy of self.  
*/
Vec3d vec3d_cliplength(Vec3d self, double m) { 
  double len  = vec3d_length(self);
  if (len <= m) return self;
  return vec3d_mul(vec3d_normalize(self), m);
}

/* Returns the vector between the two points represented by the vectors self and other. */
Vec3d vec3d_delta(Vec3d self, Vec3d other) {
  return vec3d_sub(self, other);
} 

/* Returns the distance between the two points represented by the vectors self and other. */ 
double vec3d_distance(Vec3d self, Vec3d other) {
  return vec3d_length(vec3d_delta(self, other));
} 

/* Returns the distance squared between the two points represented by the vectors self and other. */ 
double vec3d_distsq(Vec3d self, Vec3d other) {
  return vec3d_lengthsq(vec3d_delta(self, other));
} 

/* Returns true if the distance between self and other is less than or 
  equal to d.
*/
double vec3d_near_p(Vec3d self, Vec3d other, double d) {
  return vec3d_distsq(self, other) <= (d*d);
}
  

/* Returns the angle of the vector with the X axis in clockwise 
 * direction in radians
 */
double vec3d_angle(Vec3d self) { 
  return atan2(self.y, self.x);
}
 
 /* Returns a vector with components self.x, 0 */
Vec3d vec3d_xvec(Vec3d self) { 
      return vec3d(self.x, 0.0, 0.0);
}

/* Returns a vector with components 0, self.y, 0 */
Vec3d vec3d_yvec(Vec3d self) { 
      return vec3d(0.0, self.y, 0.0);
}

/* Returns a vector with components 0, self.z */
Vec3d vec3d_zvec(Vec3d self) { 
      return vec3d(0.0, 0.0, self.z);
}



/* Returns a vector with fabs applied to the components of vec. */
Vec3d vec3d_abs(Vec3d self) {
  return vec3d(fabs(self.x), fabs(self.y), fabs(self.z));
}

 
 
float vec3d_triple_dot(Vec3d v1, Vec3d v2, Vec3d v3)
{
  return vec3d_dot(v1, vec3d_cross(v2, v3));
}

Vec3d vec3d_triple_cross(Vec3d v1, Vec3d v2, Vec3d v3)
{
  return vec3d_cross(v1, vec3d_cross(v2, v3));
}
 
 
Rot3d   rot3d(double rx, double ry, double rz) {
  Rot3d result = { rx, ry, rz };
  return result;
}

Rot3d   rot3d_0() {
  return rot3d(0, 0, 0):
}

Rot3d   rot3d_add(Rot3d r1, Rot3d r2) {
  return rot3d(r1.rx + r2.rx, r1.ry + r2.ry, r1.rz + r2.rz);
}
  
Rot3d   rot3d_sub(Rot3d r1, Rot3d r2)  {
  return rot3d(r1.rx - r2.rx, r1.ry - r2.ry, r1.rz - r2.rz);
}
  
Rot3d   rot3d_mul(Rot3d r1, double factor)  {
  return rot3d(r1.rx * angle, r1.ry * angle, r1.rz * angle);
}

Rot3d vec3d_rot3d(Vec3d v1) {    
  double rx = atan2(v1.y, v1.x);  
  double ry = atan2(v1.z, v1.y);
  double rz = atan2(v1.x, v1.z);  
  return rot3d(rx, ry, rz);
}  

 
 
 
/*    
  Returns the overlap of the x component of self, of other, where  
  self and other are centers of line pieces with width rself and rother 
  respectivly. Returned as a vector with only an x component with the 
  magnitude of overlap.
Vec3d overlap_x(Vec3d self, Vec3d other,rself, rother) 
      xself   = self.vector_x
      xother  = other.vector_x
      return zero unless xself.near(xother, rself)
      return zero unless xother.near(xself, rother)
    end
*/



//  public domain function by Darel Rex Finley, 2006

//  Determines the intersection point of the line segment defined by points A and B
//  with the line segment defined by points C and D.
//
//  Returns YES if the intersection point was found, and stores that point in X,Y.
//  Returns NO if there is no determinable intersection point, in which case X,Y will
//  be unmodified.
/*
int vec3d_intersect_segment(Vec3d A, Vec3d B, Vec3d C, Vec3d D, BeVeC * result) { 

  double  dist_AB, aid_cos, aid_sin, newX, ABpos ;

  //  Fail if either line segment is zero-length.
  if (A.x == B.x && A.y == B.y || C.x == D.x && C.y == D.y) return NO;

  //  Fail if the segments share an end-point.
  if (A.x==C.x && A.y==C.y || B.x==C.x && B.y==C.y
  ||  A.x==D.x && A.y==D.y || B.x==D.x && B.y==D.y) {
    return NO; }

  //  (1) Translate the system so that point A is on the origin.
  B = vec3d_sub(B, A);
  C = vec3d_sub(C, A);
  D = vec3d_sub(D, A);
  
  
  //  Discover the length of segment A-B.
  dist_AB  = vec3d_length(B);
  
  //  (2) Rotate the system so that point B is on the positive X axis.
  aid_Cos=Bx/distAB;
  aid_sin =By/distAB;
  newX=Cx*theCos+Cy* aid_sin;
  Cy  =Cy*theCos-Cx* aid_sin; Cx=newX;
  newX=Dx*theCos+Dy* aid_sin;
  Dy  =Dy*theCos-Dx* aid_sin; Dx=newX;

  //  Fail if segment C-D doesn't cross line A-B.
  if (Cy<0. && Dy<0. || Cy>=0. && Dy>=0.) return NO;

  //  (3) Discover the position of the intersection point along line A-B.
  ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);

  //  Fail if segment C-D crosses line A-B outside of segment A-B.
  if (ABpos<0. || ABpos>distAB) return NO;

  //  (4) Apply the discovered position to line A-B in the original coordinate system.
  *X=Ax+ABpos*theCos;
  *Y=Ay+ABpos* aid_sin;

  //  Success.
  return YES; }
  */




