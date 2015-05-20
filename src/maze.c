
#include "maze.h"
#include "pointergrid.h"
#include "dynar.h"

/* A maze is a single "level" or "dungeon" in EKQ and other dungeon crawlers. 
 * A maze consists of a 3D beam-shaped grid of cubical spaces or "cubes" for 
 * short. Each 2d layer at the same eight is a "maze floor".
 * 
 * Each cube can have a floor, an optional ceiling, and up to 4 walls. There can 
 * be an optional pillar in each of the 4 corners of the cube. The cube can 
 * contain  exactly one special object. 
 * 
 * Walls are optional. A wall is visualised using a single texture. A wall can 
 * "contain" a single object such as a switch, * a lamp, a key card reader, a 
 * door, etc. Floors and ceilings are also considered to be "walls" for 
 * simplicity. 
 *   
 */
struct MazeObject_ {
  int id;
  int type;  
};

struct MazeWall_ {
  int used;
  int direction;
  int texture;
  int object;
};

struct MazePillar_ {
  int texture;
  int direction;
};

struct MazeCube_ {
  struct MazeWall_      walls[6];
  struct MazePillar_    pillars[4];
  int                   object;
  int                   flags;
};

struct MazeFloor_ {
  PointerGrid           cubes;
  int                   flags;
};

struct Maze_ {
  int                   deep;
  Dynar             *   layers;
  int                   background;
};














