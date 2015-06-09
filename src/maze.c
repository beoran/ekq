
#include "maze.h"
#include "pointergrid.h"
#include "dynar.h"
#include "store.h"

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
  if (!wall) return NULL;
  wall->used         = !0;
  wall->type         = type;
  wall->direction    = dir;
  mazewall_set_texture(wall, texture);
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










