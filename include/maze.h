#ifndef maze_H_INCLUDED
#define maze_H_INCLUDED

enum MazeDirection_ {
  MAZE_DOWN       = 0,
  MAZE_FLOOR      = 0,
  MAZE_NORTH      = 1,
  MAZE_EAST       = 2,
  MAZE_SOUTH      = 3,
  MAZE_WEST       = 4,
  MAZE_UP         = 5,  
  MAZE_CEILING    = 5,
  MAZE_RAMP_N     = 6,
  MAZE_RAMP_E     = 7,
  MAZE_RAMP_S     = 8,
  MAZE_RAMP_W     = 9,
  MAZE_DIRECTIONS = 10,
};

enum MazeWallType_ {
  MAZE_WALL_RECTANGLE    = 1,
  MAZE_WALL_TRIANGLE_UL  = 2,
  MAZE_WALL_TRIANGLE_UR  = 4,
  MAZE_WALL_TRIANGLE_DL  = 8,
  MAZE_WALL_TRIANGLE_DR  = 16,
};

typedef struct MazeItem_    MazeItem;
typedef struct MazeWall_    MazeWall;
typedef struct MazePillar_  MazePillar;
typedef struct MazeCell_    MazeCell;
typedef struct MazeFloor_   MazeFloor;
typedef struct Maze_        Maze;


Maze * maze_new(int height);
Maze * maze_free(Maze * me);
MazeFloor * maze_add_floor(Maze * me, int z, int width, int depth);
MazeCell  * maze_add_empty_cell(Maze * me, int x, int y, int z);
Maze * maze_add_cell_item(Maze * me, int x, int y, int z, int id , int type, int visual);


MazeWall * maze_add_wall(Maze * me, int z, int x, int y, int dir, int type, int texture);
MazeWall * maze_remove_wall(Maze * me, int z, int x, int y, int dir);

MazeWall * maze_set_wall_texture(Maze * me, int z, int x, int y,  int dir , int texture);
MazeWall * maze_set_wall_item(Maze * me, int z, int x, int y,  int dir, int id, int visual);
MazeWall * maze_set_wall_type(Maze * me, int z, int x, int y,  int dir, int type);




Maze * maze_load_vpath(char * vpath);
int maze_save_vpath(Maze * maze, char * vpath);



#endif




