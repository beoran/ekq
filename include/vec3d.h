#ifndef vec3d_H_INCLUDED
#define vec3d_H_INCLUDED

/* Header contains defines for a 3D vector types and related types. */ 

struct Vec3d_;
typedef struct Vec3d_         Vec3d;

/* 3D Vector */
struct Vec3d_  {  double x, y, z;  };

struct Rot3d_;
typedef struct Rot3d_         Rot3d;

/* 3D Rotation angle. Consists of 3 angles, rx, ry and rz, each representing a 
 * rotation around the given axis in radians.
 */
struct Rot3d_  {  double rx, ry, rz;  };

Vec3d   vec3d(double x, double y, double z);
Vec3d   vec3d_0(void);
Vec3d   vec3d_add(Vec3d v1, Vec3d v2);
Vec3d   vec3d_sub(Vec3d v1, Vec3d v2);
Vec3d   vec3d_mul(Vec3d v1, double factor);
#define vec3d_mult vec3d_mul

Vec3d   vec3d_neg(Vec3d v1);
double  vec3d_dot(Vec3d v1, Vec3d v2);
Vec3d   vec3d_div_unsafe(Vec3d v1, double factor);
Vec3d   vec3d_div(Vec3d v1, double factor);
double  vec3d_lengthsq(Vec3d v1);
double  vec3d_length(Vec3d v1);
Vec3d   vec3d_normalize(Vec3d v1);
Vec3d   vec3d_normalize_unsafe(Vec3d v1);
Vec3d   vec3d_project(Vec3d vec , Vec3d on);
Vec3d   vec3d_rightnormal(Vec3d vec);
Vec3d   vec3d_leftnormal(Vec3d vec);
double  vec3d_perproduct(Vec3d v1, Vec3d v2);
Vec3d   vec3d_forangle(double angle);
double  vec3d_toangle(Vec3d v1);

Vec3d   vec3d_perp(Vec3d self);
Vec3d   vec3d_nperp(Vec3d self);
Vec3d   vec3d_project_2(Vec3d self, Vec3d other);
      
Vec3d   vec3d_rotate(Vec3d self, Vec3d other);

Vec3d   vec3d_unrotate(Vec3d self, Vec3d other);
Vec3d   vec3d_lerp(Vec3d self, Vec3d other, double tx);
Vec3d   vec3d_cliplength(Vec3d self, double m);

Vec3d   vec3d_delta(Vec3d self, Vec3d other);
double  vec3d_distance(Vec3d self, Vec3d other); 
double  vec3d_distsq(Vec3d self, Vec3d other);
double  vec3d_near_p(Vec3d self, Vec3d other, double d);
double  vec3d_angle(Vec3d self);
Vec3d   vec3d_xvec(Vec3d self);
Vec3d   vec3d_yvec(Vec3d self); 
Vec3d   vec3d_abs(Vec3d self);

Vec3d vec3d_cross(Vec3d self, Vec3d other);


Rot3d   rot3d(double rx, double ry, double rz);
Rot3d   rot3d_0(void);
Rot3d   rot3d_add(Rot3d r1, Rot3d r2);
Rot3d   rot3d_sub(Rot3d r1, Rot3d r2);
Rot3d   rot3d_mul(Rot3d r1, double factor);
#define rot3d_mult rot3d_mul



#endif

