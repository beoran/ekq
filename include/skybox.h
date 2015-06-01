#ifndef skybox_H_INCLUDED
#define skybox_H_INCLUDED

#include "eruta.h"

enum SkyboxDirection_ {
  SKYBOX_DIRECTION_NORTH = 0,
  SKYBOX_DIRECTION_EAST  = 1,
  SKYBOX_DIRECTION_SOUTH = 2,
  SKYBOX_DIRECTION_WEST  = 3,
  SKYBOX_DIRECTION_UP    = 4,
  SKYBOX_DIRECTION_DOWN  = 5,
  SKYBOX_DIRECTION_MAX   = 6
};

void skybox_init(void);
void skybox_draw(void);
int skybox_set_texture(int direction, int texture);
int skybox_set_colors(int direction, ALLEGRO_COLOR colors[4]);
int skybox_set_color(int direction, int point, ALLEGRO_COLOR * color);
int skybox_set_rgb(int direction, int point, int r, int g, int b);


#endif




