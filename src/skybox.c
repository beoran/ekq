
#include "skybox.h"
#include "draw.h"
#include "store.h"

struct Skybox_ {
  ALLEGRO_BITMAP * textures[SKYBOX_DIRECTION_MAX];
  ALLEGRO_COLOR    colors[SKYBOX_DIRECTION_MAX][4];
};


static struct Skybox_ skybox;


int skybox_set_texture(int direction, int texture) {
  ALLEGRO_BITMAP * bitmap;
 
  if (direction < 0) return -1;
  if (direction >= SKYBOX_DIRECTION_MAX) return -2;
  if (texture < 0) {
    skybox.textures[direction] = NULL;
    return 0;
  }
  
  bitmap = store_get_bitmap(texture);
  if (!bitmap) return -3;
  skybox.textures[direction] = bitmap;
  return 0;
}



/* Sets the color for one of the 4 corner points of one direction of 
 * the sky box. Pass in null to set the color to white. */
int skybox_set_color(int direction, int point, ALLEGRO_COLOR * color) {
  if (direction < 0) return -1;
  if (direction >= SKYBOX_DIRECTION_MAX) return -2;
  if (point     < 0) return -3;
  if (point     > 3) return -4;
  if (color) {
    skybox.colors[direction][point] = *color; 
  } else {
    skybox.colors[direction][point] = al_map_rgb(255,255,255); 
  }
  return 0;
}

/* Sets the color for one of the 4 corner points of one direction of 
 * the sky box. Pass color by r, g, and b components from 1 to 255. 
 * No alpha channel because a sky box must always be opaque.
 */
int skybox_set_rgb(int direction, int point, int r, int g, int b) {
  ALLEGRO_COLOR color = al_map_rgb(r, g, b);
  return skybox_set_color(direction, point, &color);
}

/* Sets the colors for 4 points of one direction of the sky box. */
int skybox_set_colors(int direction, ALLEGRO_COLOR colors[4]) {
  int index;
  if (direction < 0) return -1;
  if (direction >= SKYBOX_DIRECTION_MAX) return -2;
  for (index = 0; index < 4; index++) {
    skybox_set_color(direction, index, colors + index); 
  }  
  return 0;  
}



void skybox_init(void) {
  int index;
  ALLEGRO_COLOR c4          = al_map_rgb(100, 100, 250);
  ALLEGRO_COLOR c5          = al_map_rgb(150, 150, 200);
  ALLEGRO_COLOR c6          = al_map_rgb(80 , 20, 10);
  ALLEGRO_COLOR cb          = al_map_rgb(100, 30, 30);
  ALLEGRO_COLOR cw          = al_map_rgb(255, 255, 255);  
  
  for (index = SKYBOX_DIRECTION_NORTH; index < SKYBOX_DIRECTION_MAX; index++) {
    skybox_set_texture(index, -1);
  }

  for (index = SKYBOX_DIRECTION_NORTH; index < SKYBOX_DIRECTION_UP; index++) {
    ALLEGRO_COLOR colors[4] = { c4, c4, c5, c5 };
    skybox_set_colors(index, colors);
  }
  
  {
    ALLEGRO_COLOR  ucolors[4] = { c5, c5, c5, c5 };
    skybox_set_colors(SKYBOX_DIRECTION_UP, ucolors);
  } 
  
  {
    ALLEGRO_COLOR bcolors[4] = { cb, cb, cb, cb };
    skybox_set_colors(SKYBOX_DIRECTION_DOWN, bcolors);
  }

};


/* Draws sky box and floor pane. */
void skybox_draw(void) {
  // floor pane
  draw_floor(-500, -0.1, -500, 1000, 1000, 
    skybox.colors[SKYBOX_DIRECTION_DOWN], skybox.textures[SKYBOX_DIRECTION_DOWN]
  );
    
  // sky box sides
  draw_wall(-500, 500, -500, 1000, -1000,  
    skybox.colors[SKYBOX_DIRECTION_NORTH], skybox.textures[SKYBOX_DIRECTION_NORTH]    
  );  
  
  
  draw_wall(-500, 500, 500,  1000, -1000, 
    skybox.colors[SKYBOX_DIRECTION_SOUTH], skybox.textures[SKYBOX_DIRECTION_SOUTH]
  );
  
  
  draw_wall2(-500, 500, -500, -1000, 1000,
    skybox.colors[SKYBOX_DIRECTION_WEST], skybox.textures[SKYBOX_DIRECTION_WEST]      
  );
  
  draw_wall2(500, 500, -500, -1000, 1000, 
    skybox.colors[SKYBOX_DIRECTION_EAST], skybox.textures[SKYBOX_DIRECTION_EAST]    
  );
  // sky box ceiling.
  draw_floor(-500, 500, -500, 1000, 1000,
    skybox.colors[SKYBOX_DIRECTION_UP], skybox.textures[SKYBOX_DIRECTION_UP]
  );  
}



