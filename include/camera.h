#ifndef CAMERA_H
#define CAMERA_H

#include "eruta.h"
#include "vec3d.h"
#include "inli.h"


/** Forward declaration of structs and types. */
typedef struct Camera_      Camera;
typedef struct Tracker_     Tracker;
typedef struct Panner_      Panner;  
typedef struct PannerList_  PannerList;  
typedef struct Lockin_      Lockin;  
typedef struct LockinList_  LockinList;

/** Flags for the camera. */
enum CameraFlags_ {
  /* Camera cannot move at all. */
  CAMERA_LOCK       = 1 << 1,
  /* Camera panning disabled. */
  CAMERA_NOPAN      = 1 << 2,
  /* Camera tracking disabled */
  CAMERA_NOTRACK    = 1 << 3,
  /* Camera lockin disabled. */
  CAMERA_NOLOCKIN   = 1 << 4,
  /* Camera tracking of tracked thing is immediate and locked on to tracked thing. 
  * Otherwise, the tracking is only applied gradually.
  */
  CAMERA_TRACKLOCK  = 1 << 5  
};



int camera_setflag (Camera * self , int flag );
int camera_unsetflag (Camera * self , int flag );
int camera_flag_ (Camera * self , int flag , int set );
int camera_flag (Camera * self , int flag );


Camera * camera_done (Camera * self );
Camera * camera_free (Camera * self );
Camera * camera_init(Camera * self  , float x    , float y    , float z, 
                                      float angle, float theta,
                                      float near , float far  ,
                                      float w    , float h );
                                      
Camera * camera_new(float x    , float y    , float z, 
                                 float angle, float theta,
                                 float near , float far  ,
                                 float w     , float h);

Camera * camera_update (Camera * self );

Vec3d camera_at (Camera * self );
Vec3d camera_at_x_ (Camera * self , float x );
Vec3d camera_at_y_ (Camera * self , float y );
Vec3d camera_at_z_ (Camera * self , float z );
Vec3d camera_at_xyz_ (Camera * self , float x , float y, float z);
Vec3d camera_at_(Camera * self , Vec3d at );
float camera_angle_(Camera * self, float angle);
float camera_theta_(Camera * self, float theta);

float camera_at_x(Camera * self);
float camera_at_y(Camera * self);
float camera_at_z(Camera * self);
float camera_angle(Camera * self);
float camera_theta(Camera * self);


float camera_w(Camera * self);
float camera_h(Camera * self);

Vec3d camera_speed_delta(Camera * self , float dx , float dy, float dz, float dangle, float dtheta);
Vec3d camera_speed_xyz_ (Camera * self , float x , float y, float z, float angle, float theta);
Vec3d camera_speed (Camera * self );
Vec3d camera_speed_ (Camera * self , Vec3d speed );
Camera * camera_debugprint (Camera * self);

int  camera_can_see_point_p(Camera * self , int x , int y, int z);

void camera_apply_view(Camera * self);
void camera_apply_perspective(Camera * self, ALLEGRO_DISPLAY * display);
void camera_apply_orthographic(Camera * self, ALLEGRO_DISPLAY * display);


#endif
