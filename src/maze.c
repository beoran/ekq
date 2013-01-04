
#include "maze.h"
#include "pointergrid.h"
#include "dynar.h"
#include "store.h"
#include "vec3d.h"
#include "draw.h"

/* A maze is a single "level" or "dungeon" in EKQ and other dungeon crawlers. 
 * A maze consists of a 3D beam-shaped grid of cubical spacesor "cells" for 
 * short. Each 2d layer at the same height is a "maze floor".
 * 
 * Each cell can have a floor, an optional ceiling, and up to 4 walls. 
 * The wall is either a rectangle or one of 4 possible right triangles.
 * 
 * There can be an optional pillar in each of the 4 corners of the cell. 
 * The cube can  contain exactly one special object. A cell can also contain a 
 * single "ramp" in one of either 4 directions to allow for roofs, stairs and 
 * chutes. 
 * 
 * Walls are optional. A wall is visualised using a single texture. A wall can 
 * "contain" a single object such as a switch, * a lamp, a key card reader, a 
 * door, etc. Floors and ceilings are also considered to be "walls" for 
 * simplicity. 
 *   
 */
 
 
 
struct MazeItem_ {
  int id;
  int type;  
  int visual;
};

struct MazeWall_ {
  ALLEGRO_BITMAP * texture_bmp;
  int         used;
  int         direction;
  int         texture;
  int         type;
  MazeItem    item;
};

struct MazePillar_ {
  ALLEGRO_BITMAP * texture_bmp;
  int texture;
  int direction;
};

#define MAZECELL_WALLS MAZE_DIRECTIONS

struct MazeCell_ {
  struct MazeWall_      walls[MAZECELL_WALLS];
  struct MazePillar_    pillars[4];
  int                   object;
  int                   flags;
  int                   x;
  int                   y;
  int                   visible;
};

struct MazeFloor_ {
  PointerGrid        *  cells;
  int                   flags;
  int                   z;
};

struct Maze_ {
  int                   height;
  Dynar             *   floors;
  int                   flags;
};



MazeCell * mazecell_alloc(void) {
  return calloc(1, sizeof(MazeCell));
}

MazeCell * mazecell_init(MazeCell * me, int x, int y) {
  int index;
  if (!me) return NULL;
  for (index = 0; index < MAZECELL_WALLS; index++) {
    me->walls[index].used = 0;
  }
  me->visible = 0;
  return me;
}


MazeCell * mazecell_new(int x, int y) {
  return mazecell_init(mazecell_alloc(), x, y);
}

MazeCell * mazecell_free(MazeCell * me) {
  free(me);
  return NULL;
}

void * mazecell_destroy(void * ptr) {
  return mazecell_free(ptr);
}


MazeFloor * mazefloor_done(MazeFloor * me) {
  int w, h, i, j;
  if (!me) return NULL;
  pointergrid_nullall(me->cells, mazecell_destroy);
  pointergrid_free(me->cells);
  me->cells = NULL;
  me->z     = -1000;
  me->flags = 0;
  return me;
}

MazeFloor * mazefloor_free(MazeFloor * me) {
  mazefloor_done(me);
  return NULL;
}

MazeFloor * mazefloor_alloc(void) {
  return calloc(1, sizeof(MazeFloor));
}

MazeFloor * mazefloor_init(MazeFloor * me, int z, int width, int height) {
  if (!me) return NULL;
  me->z     = z;
  me->flags = 0;
  me->cells = pointergrid_new(width, height);
  if (!me->cells) return mazefloor_free(me);
  return me;
}

MazeFloor * mazefloor_new(int z, int width, int height) {
  return mazefloor_init(mazefloor_alloc(), z, width, height);
}

Maze * maze_alloc(void) {
  return calloc(1, sizeof(Maze));
}


Maze * maze_done(Maze * me) {
  int index;
  if (!me) return NULL;
  if (!me->floors) return NULL;
    
  for (index = 0; index < me->height; index++) {
    mazefloor_free(dynar_getptr(me->floors, index));
  }
  dynar_free(me->floors);
  me->floors = NULL;
  me->height = 0;
  return me;
}

Maze * maze_free(Maze * me) {
  maze_done(me);
  free(me);
  return NULL;
}


Maze * maze_init(Maze * me, int height) {
  if (!me) return NULL;  
  me->floors = dynar_newptr(height);
  if (!me->floors) return maze_free(me);
  me->height = height;
  return me;
}


Maze * maze_new(int height) {
  return maze_init(maze_alloc(), height);
}

MazeFloor * maze_get_floor(Maze * me, int z) {
  return dynar_getptr(me->floors, z);
}

MazeFloor * maze_set_floor(Maze * me, int z, MazeFloor * floor) {
  MazeFloor  * old;
  if (!me) return NULL;
  old = maze_get_floor(me, z);
  if (old) { mazefloor_free(old); }
  return dynar_putptr(me->floors, z, floor);
}

MazeFloor * maze_add_floor(Maze * me, int z, int width, int depth) {
  MazeFloor * floor = mazefloor_new(z, width, depth);
  return maze_set_floor(me, z, floor);
}

MazeCell * mazefloor_get_cell(MazeFloor * me, int x, int y) {
  if (!me) return NULL;
  return pointergrid_fetch(me->cells, x, y);
}

MazeCell * mazefloor_set_cell(MazeFloor * me, int x, int y, MazeCell * cell) {
  MazeCell * old;
  if (!me) return NULL;
  old = mazefloor_get_cell(me, x, y);
  if (old) { mazecell_free(old); }
  pointergrid_put(me->cells, x, y, cell);
  return cell;
}

MazeCell * mazefloor_add_empty_cell(MazeFloor * me, int x, int y) {
  MazeCell * new = mazecell_new(x, y);
  if (!new) return NULL;
  return mazefloor_set_cell(me, x, y, new);
}

MazeCell * maze_add_empty_cell(Maze * me, int z, int x, int y) {
  MazeFloor * floor = maze_get_floor(me, z);
  if (!floor) return NULL;
  return mazefloor_add_empty_cell(floor, x, y);
}

MazeWall * mazecell_get_wall(MazeCell * cell, int dir) {
  if (!cell) return NULL;
  if (dir < 0) return NULL;
  if (dir > MAZECELL_WALLS) return NULL;
  return cell->walls + dir;
}

MazeCell * maze_get_cell(Maze * me, int z, int x, int y) {
  return mazefloor_get_cell(maze_get_floor(me, z), x , y);
}

MazeWall * mazefloor_get_wall(MazeFloor * floor, int x, int y, int dir) {
  MazeCell * cell = mazefloor_get_cell(floor, x, y);
  return mazecell_get_wall(cell, dir);
}

MazeWall * maze_get_wall(Maze * me, int z, int x, int y, int dir) {
  return mazefloor_get_wall(maze_get_floor(me, z), x, y, dir);
}

MazeWall * mazewall_set_texture(MazeWall * wall, int texture) {
  if (!wall) return NULL;
  wall->texture      = texture;
  wall->texture_bmp  = store_get_bitmap(wall->texture);
  return wall;
} 

MazeWall * maze_add_wall(Maze * me, int z, int x, int y, int dir, int type, int texture) {
  MazeWall * wall = maze_get_wall(me, z, x, y, dir);
  MazeCell * cell = maze_get_cell(me, z, x, y); 
  if (!wall) return NULL;
  wall->used         = !0;
  wall->type         = type;
  wall->direction    = dir;
  mazewall_set_texture(wall, texture);
  cell->visible      = !0;
  return wall;
}

MazeWall * maze_remove_wall(Maze * me, int z, int x, int y, int dir) {
  MazeWall * wall = maze_get_wall(me, z, x, y, dir);
  if (!wall) return NULL;
  wall->used         = 0;
  wall->type         = -1;
  mazewall_set_texture(wall, -1);
  return wall;
}

MazeWall * maze_set_wall_texture(Maze * me, int x, int y, int z , int dir , int texture) {
  MazeWall * wall = maze_get_wall(me, z, x, y, dir);
  return mazewall_set_texture(wall, texture);
}

MazeWall * maze_set_wall_item(Maze * me, int x, int y, int z , int dir, int id, int visual) {
  MazeWall * wall = maze_get_wall(me, z, x, y, dir);
  if (!wall) return NULL;
  wall->item.id   = id;
  return wall;  
}

int mazefloor_get_width(MazeFloor * me) {
  if (!me) return -1;
  return pointergrid_w(me->cells);
}

int mazefloor_get_depth(MazeFloor * me) {
  if (!me) return -1;
  return pointergrid_h(me->cells);
}

/** Table of rotations per direction of the wall to draw. */
Rot3d mazewall_rotations[MAZECELL_WALLS] = {
  { 0.0                     , 0.0                     , 0.0 },
  { ALLEGRO_PI / 2.0        , 0.0                     , 0.0 },
  { ALLEGRO_PI              , 0.0                     , 0.0 },
  { 3.0 * ALLEGRO_PI / 2.0  , 0.0                     , 0.0 },
  { 0.0                     , ALLEGRO_PI / 2.0        , 0.0 },
  { 0.0                     , 3.0 * ALLEGRO_PI / 2.0  , 0.0 },
  { 0.0                     , 7.0 * ALLEGRO_PI / 4.0  , 0.0 },
  { ALLEGRO_PI / 2.0        , 7.0 * ALLEGRO_PI / 4.0  , 0.0 },
  { ALLEGRO_PI              , 7.0 * ALLEGRO_PI / 4.0  , 0.0 },
  { 3.0 * ALLEGRO_PI / 2.0  , 7.0 * ALLEGRO_PI / 4.0  , 0.0 },
};


/** Table of extra translations per direction of wall to draw. */
Vec3d mazewall_translations[MAZECELL_WALLS] = {
  { 0.0                     , 0.0                     , 0.0 },
  { 2.0                     , 0.0                     , 0.0 },
  { 0.0                     , 0.0                     , 2.0 },
  { 0.0                     , 0.0                     , 0.0 },
  { 0.0                     , 2.0                     , 0.0 },
  { 0.0                     , 0.0                     , 2.0 },
  { 0.0                     , 0.0                     , 0.0 },
  { 2.0                     , 0.0                     , 0.0 },
  { 0.0                     , 0.0                     , 2.0 },
  { 0.0                     , 0.0                     , 0.0 },  
};



void mazewall_draw(MazeWall * wall, int z, int x, int y, int dir) {
  int index;
  ALLEGRO_TRANSFORM camera, model;
  const ALLEGRO_TRANSFORM * now;
  ALLEGRO_COLOR colors[4];
  Rot3d * rot;
  Vec3d * tra;
  for (index = 0; index < 4; index++) {
    colors[index] = al_map_rgb(255,255,255);
  }
  
  rot = mazewall_rotations + dir;
  tra = mazewall_translations + dir;
  
  /* Get the current transform and use it as the camera transform.  */
  now = al_get_current_transform();
  al_copy_transform(&camera, now);
  al_identity_transform(&model);
  al_rotate_transform_3d(&model, 0, 0, 1, rot->rz);
  al_rotate_transform_3d(&model, 0, 1, 0, rot->ry);
  al_rotate_transform_3d(&model, 1, 0, 0, rot->rx);
  /* swap of y and z is intentional! */  
  al_translate_transform_3d(&model, x * 2.0, z * 2.0, y * 2.0);
  al_translate_transform_3d(&model, tra->x, tra->y, tra->z);
  
  /* Compose the wall's transform with the camera transform. */
  al_compose_transform(&model, &camera);
  al_use_transform(&model);
  /* swap of y and z is intentional! */
  draw_wall(0.0, 0.0, 0.0, 2.0, 2.0, colors, wall->texture_bmp);

  /* Restore the camera transform. */
  al_use_transform(&camera);
}


void mazecell_draw(MazeCell * me, int z, int x, int y) {
  int index;
  if (!me) return;
  if (!me->visible) return;
  for (index = 0; index < MAZECELL_WALLS; index++) {
    mazewall_draw(mazecell_get_wall(me, index), z, x, y, index);
  }
}

void mazefloor_draw(MazeFloor * me, int z) {
  int i, j;
  int stopi, stopj;
  if (!me) return;
  stopi = mazefloor_get_width(me);
  stopj = mazefloor_get_depth(me);
  for (i = 0; i < stopi ; i++) {
    for (j = 0; j < stopj ; j++) {
      mazecell_draw(mazefloor_get_cell(me, i, j), z, i, j);
    }
  }
}

int maze_get_height(Maze * me) {
  if (!me) return -1;
  return dynar_size(me->floors);
}

void maze_draw(Maze * me) {
  MazeFloor floor;
  int index, stop;
  stop = maze_get_height(me);
  for (index = 0; index < stop; index++) {
    mazefloor_draw(maze_get_floor(me, index), index);
  } 
}








