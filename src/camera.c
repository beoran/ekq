#include "eruta.h"
#include "camera.h"
#include "camera_struct.h"
#include "mem.h"
#include "flags.h"
#include "monolog.h"


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
  self->field_of_view = 2 * ALLEGRO_PI * fov / 360.0;
  self->speed         = vec3d(0, 0, 0);
  self->torque        = rot3d(0, 0, 0);
  self->flags         = 0;
  self->theta         = 0.0;
  self->alpha         = 0.0;
  camera_update(self, 0.0);
  return self;
}

/** Alocates a new camera. */                                      
Camera * camera_new(Vec3d at, Vec3d look, Point size, float fov) {
  Camera * me = camera_alloc();
  return camera_init(me, at, look, size, fov);
}

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


/** Updates the camera. */
Camera * camera_update(Camera * self, double dt) {
   
   ALLEGRO_DISPLAY *display = al_get_current_display();
   double dw = al_get_display_width(display);
   double dh = al_get_display_height(display);

   
   double  f = tan(self->field_of_view / 2.0);
   
   /* Set up orthograĥic transform for UI, etc. */
   al_identity_transform(&self->orthographic_transform);
   al_orthographic_transform(&self->orthographic_transform, 0, 0, -1, dw, dh, 1000);
  
   /* Perspective transform for the main screen 3D view. */
   al_identity_transform(&self->perspective_transform);
   /* Back up camera a bit. */
   /* Set up a nice 3D view. */
   al_translate_transform_3d(&self->perspective_transform, 0, 0, -1);
   al_perspective_transform(&self->perspective_transform, 
      -1 * dw / dh * f, f, 1, f * dw / dh, -f, 1000);
 
  /*  al_perspective_transform(&projection, -1 * dw / dh * f, f,
      1,
      f * dw / dh, -f, 1000);
  */
  /* Set up the camera's position and view transform. */
  /*  al_build_camera_transform(&self->camera_transform, 
      self->position.x, self->position.y, self->position.z,
      self->look.x    , self->look.y    , self->look.z    ,
      self->up.x      , self->up.y      , self->up.z      );
     
  */  
  
  al_identity_transform(&self->camera_transform);
  al_translate_transform_3d(&self->camera_transform, 
      self->position.x, self->position.y, self->position.z);
  
  al_rotate_transform_3d(&self->camera_transform, 0, -1, 0, self->alpha); 
  al_rotate_transform_3d(&self->camera_transform, -1, 0, 0, self->theta); 

  
  /* Finally move at the set speed. */
  self->position = vec3d_add(self->position, vec3d_mul(self->speed, dt));
  return self;
 }

/** Return position of camera center. */
Vec3d camera_at(Camera * self) {
  return self->position;
}

/** Sets position by individual components. */
float camera_at_x_(Camera * self, float x) {
  return self->position.x = x;
}

/** Sets position by individual components. */
float camera_at_y_(Camera * self, float y) {
  return self->position.y = y;
}

/** Sets position by individual components. */
float camera_at_z_(Camera * self, float z) {
  return self->position.z = z;
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

/* Converts radians to degrees. */
static float rad2deg(float rad) {
  return (360 * rad) / (2 * ALLEGRO_PI);
}

/* Converts degrees to radians. */
static float deg2rad(float rad) {
  return 2 * ALLEGRO_PI * rad / 360.0;
}


/** Gets alpha angle of camera in degrees. */
float camera_alpha(Camera * self) {  
  return rad2deg(self->alpha);
}

/** Sets alpha angle of camera in degrees.  */
float camera_alpha_(Camera * self, float alpha) {
  return self->alpha = deg2rad(alpha);
}

/** Gets theta angle of camera in degrees.  */
float camera_theta(Camera * self) {
  return rad2deg(self->theta);
}

/** Sets theta angle of camera in degrees.  */
float camera_theta_(Camera * self, float theta) {
  return self->theta = deg2rad(theta);
}

/** Gets field of view of camera in degrees.  */
float camera_fov(Camera * self) {
  return rad2deg(self->field_of_view);
}


/** Sets field of view of camera.  */
float camera_fov_(Camera * self, float fov) {
  return self->field_of_view = deg2rad(fov);
}

