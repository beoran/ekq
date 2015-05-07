#include "eruta.h"
#include "camera.h"
#include "camera_struct.h"
#include "mem.h"
#include "flags.h"
#include "monolog.h"

/* 

Ideas about camera tracking, panning and limiting.

Normally the camera tracks or follows a single mobile Thing.
Normally, the thing is only tracked if it goes out of
sight of the camera. However, it is also possible to lock 
on to the Thing exactly so the camera moves completely with 
it. 

It's possible to change the thing that is being tracked. 
This transition can be immediate or smooth. The speed
of this transition can be set.

Sometimes the camera pans around. It moves it's center between 
a list of goal points. This motion can be immediate or smooth.
The speed of this transition may be set. Panning has priority
over tracking. It may be a good idea to implement
the transition between two things as immediate, but set up a panning
from the old thing's position to the new thing's one. 

On top of that, the motion of the camera is limited by 
the limits of the current tile map, and possibly
by special tiles that prevent the camera to pan or 
scroll or track past them. 

These map limits may be disabled individually in any 4 directions,
The effect of the scroll lock tiles may be also be disbled.

Finally, the camera may be completely locked in to place. 
This could be used, for example, in battle mode.

*/

/** Sets an individual flag on the Panner. */
int panner_setflag(Panner * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the Panner. */
int panner_unsetflag(Panner * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the Panner. 
If set is true the flag is set, if false it's unset. */
int panner_flag_(Panner * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set. */
int panner_flag(Panner * self, int flag) {
  return flags_get(self->flags, flag);
}

#define PANNER_SPEED_DEFAULT 10.0

/** Initializes a panner and sets it to be active. If speed is negative
or zero it will be replaced by 10.0 */
Panner * panner_init(Panner * self, Point goal, float speed, int immediate) {
  if(!self) return NULL;
  self->goal  = goal;
  self->speed = speed;
  if(self->speed <= 0) { self->speed = PANNER_SPEED_DEFAULT; }
  panner_flag_(self, PANNER_IMMEDIATE, immediate);
  panner_flag_(self, PANNER_ACTIVE, true);
  return self;
}

/** Cleans up a panner after use. */
Panner * panner_done(Panner * self) {
  if(!self) return NULL;
  self->goal  = bevec0();
  self->speed = 0.0;
  return self;
}


/** Allocates a new panner list node. */
PannerList * pannerlist_new(Point goal, float speed, int immediate) {
  PannerList * self = STRUCT_ALLOC(PannerList);
  panner_init(&self->panner, goal, speed, immediate);
  inli_init(&self->list);
  return self;
}

/** Frees a panner list node, also removes it from 
the intrusive list it was in if that was the case.*/ 
PannerList * pannerlist_free(PannerList * self) {
  if (!self) { return NULL ; }
  inli_remove(&self->list);
  panner_done(&self->panner);
  return mem_free(self);
}

/** Frees all nodes of a PannerList */
PannerList * pannerlist_freeall(PannerList * self) {
  PannerList * index, * next;
  index = self;
  while (index) {
    Inli * nextlist = inli_next(&index->list);
    next = INLI_LISTDATA(nextlist, PannerList);
    pannerlist_free(index);
    index = next;
  }
  return NULL;
}

/** Initializes a lockin and sets it to be active.  */
Lockin * lockin_init(Lockin * self, float x, float y, float w, float h) {
  if(!self) return NULL;
  self->box   = rebox_new(x, y, w, h);
  self->flags = LOCKIN_ACTIVE;
  return self;
}

/** Cleans up a lockin after use. */
Lockin * lockin_done(Lockin * self) {
  if(!self) return NULL;
  self->flags = 0;
  return self;
}


/** Allocates a new lockin list node. */
LockinList * lockinlist_new(float x, float y, float w, float h) {
  LockinList * self = STRUCT_ALLOC(LockinList);
  lockin_init(&self->lockin, x, y, w, h);
  inli_init(&self->list);
  return self;
}

/** Frees a lockin list node, also removes it from 
the intrusive list it was in if that was the case.*/ 
LockinList * lockinlist_free(LockinList * self) {
  if (!self) { return NULL; }
  inli_remove(&self->list);
  lockin_done(&self->lockin);
  return mem_free(self);
}

/** Frees all nodes of a LockinList */
LockinList * lockinlist_freeall(LockinList * self) {
  LockinList * index, * next;
  index = self;
  while (index) {
    Inli * nextlist = inli_next(&index->list);
    next = INLI_LISTDATA(nextlist, LockinList);
    lockinlist_free(index);
    index = next;
  }
  return NULL;
}

/** Sets an individual flag on the Camera. */
int camera_setflag(Camera * self, int flag) {
  return flags_set(&self->flags, flag);
}

/** Unsets an individual flag on the Camera. */
int camera_unsetflag(Camera * self, int flag) {
  register int wflags = self->flags;
  return flags_unset(&self->flags, flag);
}

/** Sets or unsets an individual flag on the Camera. 
If set is true the flag is set, if false it's unset. */
int camera_flag_(Camera * self, int flag, int set) {
  return flags_put(&self->flags, flag, set);
}

/** Checks if an individual flag is set. */
int camera_flag(Camera * self, int flag) {
  return flags_get(self->flags, flag);
}

Camera * camera_alloc() {
  return STRUCT_ALLOC(Camera);
}


/** Cleans up a camera. */
Camera * camera_done(Camera * self) {
  if(!self) { return NULL; } 
  camera_freelockins(self);
  camera_freepanners(self);
  return self;
}

/** Frees the camera after use. */
Camera * camera_free(Camera * self) {
  camera_done(self);
  STRUCT_FREE(self);
  return NULL;
}

/** Initializes a camera. */
Camera * camera_init(Camera * self, Vec3d at, Vec3d look, Point size, float fov) {
  if(!self) return NULL;
  self->position      = at;
  self->size          = size;
  self->look          = look;
  self->up            = vec3d(0, 1, 0);
  self->field_of_view = fow;
  self->speed         = vec3d(0, 0, 0);
  self->torque        = vec3d(0, 0, 0);
  self->flags         = 0;
  camera_update(self);
  return self;
}

                                      
Camera * camera_new(Vec3d at, Vec3d look, Point size, float fov);

Camera * camera_debugprint (Camera * self);

/** Applies the camera's position and point of view transformation. */
void camera_apply_view(Camera * self) {
  al_use_transform(&self->camera_transform);
}

/** Applies a perspective transformation */
void camera_apply_perspective(Camera * self, ALLEGRO_DISPLAY * display) {
  al_use_projection_transform(&self->perspective_transform);
}

/** Applies an orthographic transformation for the UI. */
void camera_apply_orthographic(Camera * self, ALLEGRO_DISPLAY * display) {
  al_use_projection_transform(&self->orthographic_transform);
}

/** Alocates a new camera. */
Camera * camera_new(float x, float y, float w, float h) {
  Camera * self = camera_alloc();
  return camera_init(self, x, y, w, h);
}

/** Updates the camera. */
Camera * camera_update(Camera * self, double dt) {
   
   double dw = self->size.x;
   double dh = self->size.y;
   double  f = tan(self->field_of_view / 2.0);
   
   /* Set up orthograĥic transform for UI, etc. */
   al_identity_transform(&self->orthographic_transform);
   al_orthographic_transform(&self->orthographic_transform, 0, 0, -1, dw, dh, 1);
  
   /* Perspective transform for the main screen 3D view. */
   al_identity_transform(&self->perspective_transform);
   /* Back up camera a bit. */
   al_translate_transform_3d(&self->perspective_transform, 0, 0, -1);
   /* Set up a nice 3D view. */
   al_perspective_transform(&self->perspective_transform, 
      -1 * dw / dh *f, f, 1, f * dw / dh, -f, 1000);
   );
  
  /* Set up the camera's position and view transform. */
   al_build_camera_transform(&self->camera_transform, 
      self.position.x, self.position.y, self.position.z,
      self.look.x    , self.look.y    , self.look.z    ,
      self.up.x      , self.up.y      , self.up.z      );
     
    
  /* Finally move at the set speed. */
  self->position = bevec_add(self->position, bevec_mul(self->speed, dt));
  return self;
 }

/** Return position of camera center. */
Vec3d camera_at(Camera * self) {
  return self->position;
}

/** Sets position by individual components. */
Vec3d camera_at_x_(Camera * self, float x) {
  self->position.x = x;
  return self->position;
}

/** Sets position by individual components. */
Vec3d camera_at_y_(Camera * self, float y) {
  self->position.y = y;
  return self->position;
}

/** Sets position by individual components. */
Vec3d camera_at_z_(Camera * self, float z) {
  self->position.z = z;
  return self->position;
}

/** Sets position of camera. */
Vec3d camera_at_(Camera * self, Vec3d at) {
  return self->position  = at;
}

/** Sets position by individual components. */
Vec3d camera_at_xyz_(Camera * self, float x, float y, float z) {
  return self->position = vec3d(x, y, z);
}

/** Return x position of camera's position. */
float camera_at_x(Camera * self) {
  return  self->position.x;
}

/** Return x position of camera's position. */
float camera_at_y(Camera * self) {
  return  self->position.y;
}

/** Return x position of camera's position. */
float camera_at_z(Camera * self) {
  return  self->position.z;
}

/** Return width of the camera's screen size. */
float camera_w(Camera * self) {
  return self->size.x;
}

/** Return height of the camera's screen size. */
float camera_h(Camera * self) {
  return self->size.y;
}


/** Modifies speed by individual components. */
Vec3d camera_speed_delta_xyz(Camera * self, float dx, float dy, float dz) {
  self->speed.x += dx;
  self->speed.y += dy;
  self->speed.z += dz; 
  return self->speed;
}

/** Sets speed by individual components. */
Vec3d camera_speed_xyz_(Camera * self, float x, float y, float z) {
  self->speed.x = x;
  self->speed.y = y;
  self->speed.z = z;  
  return self->speed;
}

/**  Gets speed. */
Vec3d camera_speed(Camera * self) {  
  return self->speed;
}

/** Sets speed.  */
Vec3d camera_speed_(Camera * self, Vec3d speed) {
  self->speed = speed;
  return self->speed;
}

/** Gets rotation speed. */
Rot3d camera_torque(Camera * self) {  
  return self->torque;
}

/** Sets rotation speed.  */
Rot3d camera_torque_(Camera * self, Rot3d speed) {
  self->torque = speed;
  return self->torque;
}


