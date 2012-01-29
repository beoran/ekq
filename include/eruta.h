#ifndef ERUTA_H
#define ERUTA_H

/** Commnly used includes. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <lua.h>
#include <lauxlib.h>
#include <chipmunk.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

/** Screen size wich will always be 640x480 (real or virtually). */
#define SCREEN_W      640
#define SCREEN_H      480



#ifndef TRUE
#define TRUE (!0)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

/** Boolean type*/
#ifndef BOOL
#define BOOL int
#endif

/** String type */
#ifndef STR
#define STR ALLEGRO_USTR
#endif


/** Use the chipmunk vector as the Point type.
Allegro also uses floats now for it's drawing. */
typedef cpVect Point;

/** Wrapper macros */
#define point(X, Y) cpv(X, Y);

/** Commonly used types and typedefs. */

struct Program_;
typedef struct Program_ Program;


enum ProgramMode_ {
  // Negatieve modes are special.
  ProgramModeQuit  = -2, 
  ProgramModeNone  = -1,
  ProgramModeIntro = 0,
  ProgramModeStart = 1,
  ProgramModePlay  = 2,
  ProgramModeMenu  = 3,
  ProgramModeEdit  = 4,
  ProgramModeRes1  = 5,
  ProgramModeRes2  = 6,
  ProgramModeRes3  = 7,
  ProgramModeLast  = 8,
};

typedef enum ProgramMode_ ProgramMode;




#endif