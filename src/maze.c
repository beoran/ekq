
#include "maze.h"
#include "pointergrid.h"
#include "dynar.h"
#include "store.h"
#include "vec3d.h"
#include "draw.h"
#include "fifi.h"
#include "monolog.h"

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

MazeFloor * mazefloor_init(MazeFloor * me, int z, int width, int depth) {
  if (!me)            return NULL;
  if (width < 1)      return NULL;
  if (depth < 1)      return NULL;
  if (width > 10000)  return NULL;
  if (depth > 10000)  return NULL;
  
  me->z     = z;
  me->flags = 0;
  me->cells = pointergrid_new(width, depth);
  if (!me->cells) return mazefloor_free(me);
  return me;
}

MazeFloor * mazefloor_new(int z, int width, int height) {
  MazeFloor * me;
  me = mazefloor_alloc();
  if (!mazefloor_init(mazefloor_alloc(), z, width, height)) {
    return mazefloor_free(me);
  }
  return me;
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


/* Saving and loading of the maze uses an Allegro config file 
 * for simplicity. It's almost as flexible as xml but not so heavy. */

#define SAVE_TO_CONFIG(CONFIG, SECTION, OBJ, KEY, FORMAT, BUFSIZE)    \
  do { char _value[BUFSIZE]; sprintf(_value, FORMAT, (OBJ)->KEY);     \
  al_set_config_value(CONFIG, SECTION, #KEY, _value);                 \
  } while (0)
    
#define SAVE_CONST_TO_CONFIG(CONFIG, SECTION, KEY, VALUE, FORMAT, BUFSIZE)    \
  do { char _value[BUFSIZE]; sprintf(_value, FORMAT, VALUE);          \
  al_set_config_value(CONFIG, SECTION, #KEY, _value);                 \
  } while (0)
    
#define SAVE_VAR_TO_CONFIG(CONFIG, SECTION, VAR, FORMAT, BUFSIZE)    \
  do { char _value[BUFSIZE]; sprintf(_value, FORMAT, VAR);            \
  al_set_config_value(CONFIG, SECTION, #VAR, _value);                 \
  } while (0)



int mazewall_save_to_config(MazeWall * me, ALLEGRO_CONFIG * config, int z, int x, int y, int dir) {
  /* A 64 bytes int is at most 20 characters long when printed out, 
   * so we can use a static buffer */
  char section[100] = { '\0' };
  
  sprintf(section, "maze wall %d %d %d %d", z, x, y, dir);
  al_add_config_section(config, section);
  
  SAVE_TO_CONFIG(config, section, me, used, "%d", 100);
  
  /* Only save details of the wall if in use. */
  if (!me->used) return !0;
  
  SAVE_TO_CONFIG(config, section, me, direction   , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, texture     , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, type        , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, type        , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, item.id     , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, item.type   , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, item.visual , "%d", 100);
  
  return !0;
  
}


int mazecell_save_to_config(MazeCell * me, ALLEGRO_CONFIG * config, int z, int x, int y) {
  int index;
  char section[100] = { '\0' };
  if (!me) return !0;
    
  sprintf(section, "maze cell %d %d %d", z, x, y);
  al_add_config_section(config, section);
  
  SAVE_TO_CONFIG(config, section, me, x     , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, y     , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, flags , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, object, "%d", 100);
  SAVE_CONST_TO_CONFIG(config, section, walls, MAZECELL_WALLS, "%d", 100);
    
  for (index = 0; index < MAZECELL_WALLS; index++) {
    mazewall_save_to_config(mazecell_get_wall(me, index), config, z, x, y, index);
  }
  
  return !0;
}


/*
 * 
 * struct MazeFloor_ {
};

struct Maze_ {
  int                   height;
  Dynar             *   floors;
  int                   flags;
};

*/

int mazefloor_save_to_config(MazeFloor * me, ALLEGRO_CONFIG * config, int z) {
  int i, j, width, depth;
  char section[100] = { '\0' };
  if (!me) return !0;
  
  width = mazefloor_get_width(me);
  depth = mazefloor_get_depth(me);
    
  sprintf(section, "maze floor %d", z);
  al_add_config_section(config, section);

  SAVE_VAR_TO_CONFIG(config, section, width, "%d", 100);  
  SAVE_VAR_TO_CONFIG(config, section, depth, "%d", 100);
  SAVE_TO_CONFIG(config, section, me, z     , "%d", 100);
  SAVE_TO_CONFIG(config, section, me, flags , "%d", 100);
    
  for (i = 0; i < width; i++) {
    for (j = 0; j < depth; j++) { 
      mazecell_save_to_config(mazefloor_get_cell(me, i, j), config, z, i, j);
    }
  }
  
  return !0;
}

 
int maze_save_to_config(Maze * me, ALLEGRO_CONFIG * config) {
  int index, height;
  char section[100] = { '\0' };
  if (!me) return 0;
  
  height = maze_get_height(me);
  
  al_add_config_section(config, "maze");

  SAVE_VAR_TO_CONFIG(config, section, height, "%d", 100);  
  SAVE_TO_CONFIG(config, section, me, flags , "%d", 100);
    
  for (index = 0; index < height; index++) {
    mazefloor_save_to_config(maze_get_floor(me, index), config, index);
  }
  
  return !0;
}


#define LOAD_FROM_CONFIG(CONFIG, SECTION, OBJ, KEY, FORMAT, DEFAULT)    \
  do { const char * _value;                                             \
  _value = al_get_config_value(CONFIG, SECTION, #KEY);                  \
  if (_value) { sscanf(_value, FORMAT, &(OBJ->KEY)); }                  \
  else { OBJ->KEY = DEFAULT; }                                          \
  } while (0)
        
#define LOAD_VAR_FROM_CONFIG(CONFIG, SECTION, VAR, FORMAT, DEFAULT)     \
  do { const char * _value;                                             \
  _value = al_get_config_value(CONFIG, SECTION, #VAR);                  \
  if (_value) { sscanf(_value, FORMAT, &(VAR)); }                       \
  else { VAR = DEFAULT; }                                               \
  } while (0)



MazeWall * mazewall_load_from_config(MazeWall * me, ALLEGRO_CONFIG * config, int z, int x, int y, int dir) {
  /* A 64 bytes int is at most 20 characters long when printed out, 
   * so we can use a static buffer */
  char section[100] = { '\0' };
  int used;
  
  sprintf(section, "maze wall %d %d %d %d", z, x, y, dir);
  
  LOAD_FROM_CONFIG(config, section, me, used        , "%d", 0);
  LOAD_FROM_CONFIG(config, section, me, direction   , "%d", -1);
  LOAD_FROM_CONFIG(config, section, me, texture     , "%d", -1);
  LOAD_FROM_CONFIG(config, section, me, type        , "%d", -1);
  LOAD_FROM_CONFIG(config, section, me, item.id     , "%d", -1);
  LOAD_FROM_CONFIG(config, section, me, item.type   , "%d", -1);
  LOAD_FROM_CONFIG(config, section, me, item.visual , "%d", -1);
  
  if (me->texture > 0) {
    mazewall_set_texture(me, me->texture);
  }
  
  if (me->direction != dir) {
    LOG_ERROR("Maze wall not correct: %s", section);
    return NULL;
  }
  
  return me;
  
}


MazeCell * mazecell_load_from_config(ALLEGRO_CONFIG * config, int z, int x, int y) {
  int index, walls;
  char section[100] = { '\0' };
  MazeCell * me;
  me = mazecell_new(x, y); 
  if (!me) return NULL;
    
  sprintf(section, "maze cell %d %d %d", z, x, y);
  
  LOAD_FROM_CONFIG(config, section, me, x     , "%d", -1);
  LOAD_FROM_CONFIG(config, section, me, y     , "%d", -1);
  LOAD_FROM_CONFIG(config, section, me, flags , "%d", 0);
  LOAD_FROM_CONFIG(config, section, me, object, "%d", -1);
  LOAD_VAR_FROM_CONFIG(config, section, walls , "%d", 0);
  
  if (
      (walls != MAZECELL_WALLS)
    || (me->x != x)
    || (me->y != y)
  ) {
    LOG_ERROR("Maze cell not correct: %s", section);
    return mazecell_free(me);
  }
     
  for (index = 0; index < MAZECELL_WALLS; index++) {
    if (!mazewall_load_from_config(mazecell_get_wall(me, index), config, z, x, y, index)) {
      return NULL;
    }
  }
  
  return me;
}


/*
 * 
 * struct MazeFloor_ {
};

struct Maze_ {
  int                   height;
  Dynar             *   floors;
  int                   flags;
};

*/

MazeFloor * mazefloor_load_from_config(ALLEGRO_CONFIG * config, int z) {
  int i, j, width, depth;
  char section[100] = { '\0' };
  MazeFloor * me;
  sprintf(section, "maze floor %d", z);
 
  LOAD_VAR_FROM_CONFIG(config, section, width, "%d", -1);  
  LOAD_VAR_FROM_CONFIG(config, section, depth, "%d", -1);
 
  me = mazefloor_new(z, width, depth);
  if (!me) { 
    LOG_ERROR("Maze floor not correct: %s\n", section);
    return NULL;
  } 
  
  LOAD_FROM_CONFIG(config, section, me, z     , "%d", -1);
  LOAD_FROM_CONFIG(config, section, me, flags , "%d", 0);
  
  if (me->z != z) { 
    LOG_ERROR("Maze floor not correct: %s\n", section);
    return mazefloor_free(me);
  } 
  
  for (i = 0; i < width; i++) {
    for (j = 0; j < depth; j++) { 
      MazeCell * cell = mazecell_load_from_config(config, z, i, j);
      /* Missing/NULL cells are fine here */
      mazefloor_set_cell(me, i, j, cell);
    }
  }
  
  return me;
}

 
Maze * maze_load_from_config(ALLEGRO_CONFIG * config) {
  int index, height;
  char * section = "maze";
  Maze * me;
  
  LOAD_VAR_FROM_CONFIG(config, section, height, "%d", -1);  
  if ((height < 1) || (height > 10000)) { 
    LOG_ERROR("Maze cell not height correct: %s", section);
    return NULL;
  }
  
  me = maze_new(height);
  
  if (!me) { 
    LOG_ERROR("Maze cell not correct or out of memory: %s\n", section);
    return NULL;
  }
  
  LOAD_FROM_CONFIG(config, section, me, flags , "%d", 0);
    
  for (index = 0; index < height; index++) {
    MazeFloor * floor = mazefloor_load_from_config(config, index);
    maze_set_floor(me, index, floor);
  }
  
  return me;
}

Maze * maze_load_filename(char * filename) {
  Maze * maze;
  ALLEGRO_CONFIG * config;
  config = al_load_config_file(filename);
  if (!config) { 
    LOG_ERROR("Cannot open config file %s\n", filename);
    return NULL; 
  }
  maze = maze_load_from_config(config);
  al_destroy_config(config);
  return maze;
}

int maze_save_filename(Maze * maze, char * filename) {
  ALLEGRO_CONFIG * config;
  config = al_create_config();  
  if (!config) { 
    LOG_ERROR("Cannot create config for file %s\n", filename);
    return 0; 
  }
  
  if (!maze_save_to_config(maze, config)) {
    LOG_ERROR("Cannot covert maze to config for file %s\n", filename);
    al_destroy_config(config);
    return 0;
  }
  
  if (!al_save_config_file(filename, config)) {
    LOG_ERROR("Cannot save maze to config file for file %s\n", filename);
    al_destroy_config(config);
    return 0;
  }
  
  return !0;
}


/**
* Loads a maze with the given vpath
*/
Maze * maze_load_vpath(char * vpath) {
  return fifi_loadsimple_vpath(
            (FifiSimpleLoader *)maze_load_filename, vpath);
}


/**
* Loads a maze with the given vpath, which is the REAL path!
*/
int maze_save_vpath(Maze * maze, char * vpath) {
  return maze_save_filename(maze, vpath);
}


