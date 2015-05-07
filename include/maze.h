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
};

typedef struct MazeObject_  MazeObject;
typedef struct MazeWall_    MazeWall;
typedef struct MazePillar_  MazePillar;
typedef struct MazeCube_    MazeCube;
typedef struct MazeFloor_   MazeFloor;
typedef struct Maze_        Maze;




#endif




