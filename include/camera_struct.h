#ifndef CAMERA_STRUCT_H_INCLUDED
#define CAMERA_STRUCT_H_INCLUDED

#include "vec3d.h"
#include "inli.h"

/** The Camera is one (or more if using split screen) of the
* rectangular views that the player has on the game world.
**/
struct Camera_ {
  /* The main camera transform */
  ALLEGRO_TRANSFORM     camera_transform;

  /* The camera's perspective transform. */
  ALLEGRO_TRANSFORM     perspective_transform;
  /* The camera's ortograhpic transform. */
  ALLEGRO_TRANSFORM     orthographic_transform;
  ALLEGRO_TRANSFORM     orthographic_view;  
  
  /* Position of vector. */
  Vec3d                 position;
  /* Direction camera is looking at. */
  Vec3d                 look;
  /* Up vector of the world. */
  Vec3d                 up;
  /* Angle around Y axis. */
  float                 alpha;
  /* Angle around X axis. */
  float                theta;
  /* Vertical field of view in radians. */
  double                field_of_view; 
  /* Speed of motion. */
  Vec3d                 speed;
  /* Rotational speed. */
  Rot3d                 torque;
  
  /* Size of view. */
  Point                 size;
  
  /* Various flags*/
  int                   flags;
};


#endif
