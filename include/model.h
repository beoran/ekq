#ifndef model_H_INCLUDED
#define model_H_INCLUDED

#include <stdio.h>

/* 3D modeling.  */

/* A simple 3D model. May only have a single texture. */
typedef struct Model_ Model;

Model * model_new();
Model * model_init(Model * me);
Model * model_done(Model * me);
Model * model_free(Model * me);

int model_add_vertex(Model * me, float x, float y, float z);
int model_add_triangle(Model * me, int i1, int i2, int i3);
int model_add_uv(Model * me, float u, float v);

int model_set_uv(Model * me, int index, float u, float v);
int model_set_texture(Model * me, int texture);

int model_set_scale(Model * me, float sx, float sy, float sz);
int model_set_position(Model * me, float x, float y, float z);
int model_set_speed(Model * me, float vx, float vy, float vz);
int model_set_rotation(Model * me, float rx, float ry, float rz);
int model_set_rgba(Model * me, int index, int r, int g, int b, int a);

void model_update(Model * me, double dt);

void model_draw(Model * me);

Model * model_load_obj_file(FILE * file); 
Model * model_load_obj_filename(char * filename); 
Model * model_load_obj_vpath(char * vpath); 




#endif




