#include "eruta.h"
#include "mem.h"
#include "dynar.h"
#include "silut.h"
#include "tile.h"

#ifndef TILE_MAXFRAME
#define TILE_MAXFRAME 32
#endif

#ifndef TILE_MAXPROGRAM
#define TILE_MAXPROGRAM 64
#endif

/**
* A tile set 
*/
struct Tileset_ {
  Dynar    * tiles;   
  // Tile     * tiles;
  Image    * sheet;
  // size_t     size;
  size_t     last;
  int        w;
  int        h;
};

/** 
* A single tile from a tile map.
* Tiles can be animated. This works like this: a tile has an animation
* pointer and offset which points to the next tile to be drawn in the tileset.
*
* A tile can only hold up to TILE_FRAMES tiles pointers in itself.
*/
struct Tile_ {
  Tileset     * set;  
  int           index;
  /* Tileset this tile belongs to + index in the tile set. */  
  int           flags;
  /* Information about the tile's properties. */
  int           kind;
  /* Index of currently active animation pointer for this tile. */
  int           anim;
  /* Offset to the tile to skip to when animating. If this is
  0 the tile is not animated. If nonzero, the tile will skip to
  the tile in the same tile set set with index index + anim.
  May be negative to "roll back" an animation to it's begin. */  
  int           active;
  /* For unanimated tiles, now is set to the index of the tile itself.
   For animated tiles, it is set to the index of tile that currently should
   be displayed in stead of this tile due to animation.
  */
  double        wait;
  /* Time in s to wait before jumping to the next frame of this tile. */  
  double        time;
  /* Time since last animation in s. */
  /* Sub-position in the tile sheet image of the tileset. */
  Point         now;
};

/* NOTE: Tiles could be implemented using sub bitmaps as they seem to be
* slightly faster if they are preallocated. however the speed gain would
* be around 2%, so it's not a priority yet. It could simplify some of
* the code, though, but I'll use sub-bitmaps for spites first.
*/


/** Cleans up a tileset, and empties it.  */
void tileset_done(Tileset * set) {
  if (set->tiles) {
    dynar_free(set->tiles);
    set->tiles = NULL;
    set->w      = -1;
    set->h      = -1;
  }
}

/** Deallocates a tileset */
void tileset_free(Tileset * set) {
  tileset_done(set);
  mem_free(set);
}

/** Retuns the amount of tiles in the tile set. */
int tileset_size(Tileset * set) {
  return dynar_size(set->tiles);
}

/** Initializes a given tileset with a given bitmap tile sheet */
Tileset * tileset_init(Tileset * set, Image * sheet) {
  int size = 0, index = 0;
  if(!set)      return NULL;
  set->sheet    = sheet;
  if(!set->sheet)    {
    set->w      = -1;
    set->h      = -1;    
    set->tiles  = NULL;
    return NULL;
  }
  // alow re-init
  if (set->tiles) {
    tileset_done(set);
  } 
  set->w        = image_w(set->sheet);
  set->h        = image_h(set->sheet);
  size          = (set->w / TILE_W) * (set->h / TILE_H);
  set->tiles    = dynar_new(size, sizeof(Tile));
  set->last     = 0;
  // set->tiles    = mem_alloc(sizeof(Tile) * set->size);
  if (!set->tiles) {
    set->w      = -1;
    set->h      = -1;    
    return NULL;
  }
  // now set up the tiles
  for(index = 0; index < dynar_size(set->tiles); index ++) {
    tile_init(tileset_get(set, index), set, index);
  }    
  return set;
}

/** Creates a new tileset with the given tile sheet image. */
Tileset * tileset_new(Image * sheet) {
  Tileset * set, * result;
  set                         = STRUCT_ALLOC(Tileset);
  if(!set) return NULL;
  result                      = tileset_init(set, sheet);
  if(!result) {
     tileset_free(set);
     return NULL;
  }
  return result;
}


/*Macros that calculate the position of a tile in a tile set's sheet. */
#define TILE_SHEET_Y(TILE, SET)\
        (((TILE->active * TILE_W) / ((SET)->w)) * TILE_H)

#define TILE_SHEET_X(TILE, SET)\
        ((TILE->active * TILE_W) % ((SET)->w))
  
/** Recalculates the tile's position (now) in it's tile set. */
Tile * tile_recalculate(Tile * tile) {
  int x, y;
  if(!tile) return NULL;
  if(!tile->set) return NULL;
  x = TILE_SHEET_X(tile, tile->set);
  y = TILE_SHEET_Y(tile, tile->set);
  tile->now = point(x, y); 
  return tile;
}

/** Initializes a tile to belong to a given tile set. */
Tile * tile_init(Tile * tile, Tileset * set, int index) {
  if(!tile) return NULL;
  if(!set) return NULL;
  tile->index   = index;
  tile->set     = set;
  tile->anim    = 0;
  tile->time    = 0.0;
  tile->wait    = 0.250;
  tile->active  = index;
  tile_recalculate(tile);
  return tile;
}

/** Gets a tile from a tile set by it's tile id. **/ 
Tile * tileset_get(Tileset * set, int index) {
  if(!set) return NULL;
  if (index < 0) return NULL;
  return dynar_getdata(set->tiles, index);
}

/** Sets the animation parameter of this tile */
Tile * tile_anim_(Tile * tile, int anim) {
  if(!tile) return NULL;
  tile->anim = anim;
  return tile;
}

/** Gets the animation parameter of this tile, or 0 if NULL */
int tile_anim(Tile * tile) {
  if(!tile) return 0;
  return tile->anim;
}

/** Sets the wait parameter of this tile in ms */
Tile * tile_wait_(Tile * tile, int wait) {
  if(!tile) return NULL;
  tile->wait = (double)wait / 1000.0;
  return tile;
}

/** Gets the wait parameter of this tile in ms, or -1 if NULL */
int tile_wait(Tile * tile) {
  if(!tile) return -1;
  return (int)(tile->wait * 1000.0);
}


/* Helper lookup table for the tile flag names */
static Silut tile_flagnames[] = {
  { TILE_WALL   , "wall"    },
  { TILE_WATER  , "water"   },
  { TILE_LEDGE  , "ledge"   },
  { TILE_STAIR  , "stair"   },
  { TILE_PUSH   , "push"    },
  { TILE_NORTH  , "north"   },
  { TILE_SOUTH  , "south"   },
  { TILE_EAST   , "east"    },
  { TILE_WEST   , "west"    },
  SILUT_DONE  
};

/** Gets the value of the flags of a tile. */
int tile_flags(Tile * tile) {
  if(!tile) return 0;
  return tile->flags;
}

/** Sets the flags on a tile. */
Tile * tile_flags_(Tile * tile, int flags) {
  if(!tile) return NULL;
  tile->flags = flags;
  return tile;
}

/** Sets a single flag on a tile. */
Tile * tile_setflag(Tile * tile, int flag) {
  if(!tile) return NULL;
  tile->flags = BIT_SETFLAG(tile->flags, flag);
  return tile;
}

/** Removes a single flag on a tile. */
Tile * tile_unflag(Tile * tile, int flag) {
  if(!tile) return NULL;
  tile->flags = BIT_UNFLAG(tile->flags, flag);
  return tile;
}

/** Checks a single flag on a tile. */
int tile_isflag(Tile * tile, int flag) {
  if(!tile) return 0;
  return BIT_ISFLAG(tile->flags, flag);
}

/** Sets a tile's flags from a property string.
* This uses an internal lookup table.
*/
Tile * tile_property_(Tile * tile, char * property) {
  if(!tile) return NULL;
  Silut * aid = silut_lsearchcstr(tile_flagnames, property);
  if(!aid) return NULL;  
  return tile_setflag(tile, aid->integer);
}

/** Rewinds a tile's animations. */
void tile_rewindanime(Tile * tile) {
  if (!tile) return;
  tile->active  = tile->index;
  // Finally recalculate tile position.
  tile_recalculate(tile);
}

/** Updates a tile to animate it. Ignores dt for now. */
void tile_update(Tile * tile, double dt) {
  int active = 0;
  Tile * aidtile = NULL;
  Tile * nowtile = tileset_get(tile->set, tile->active);
  // nowtile is the tile that is currently active, that is shown.
  // in stead of ourself, but it also may be ourself.
  if(!nowtile) return;
  tile->time    += dt; // advance animation time of tile. 
  // Don'tanimate if not enough time has passed
  if(tile->time < tile->wait) return;
  // if we get here, reset animation time.
  tile->time     = 0.0;
  // take the animation parameter and add it to the active  
  active  = tile->active + nowtile->anim;
  aidtile = tileset_get(tile->set, active);
  // Check if there is such a tile.
  if(!aidtile) return;
  // If there is no such tile, don't change the active tile of this tile.
  tile->active = active;  
  // Finally recalculate tile position.
  tile_recalculate(tile);
}

/** Updates all tiles in a tile set so they all get animated. */
void tileset_update(Tileset * set, double dt) {
  int index;
  if (!set) return;
  if (!set->tiles) return;
  for (index = 0; index < tileset_size(set) ; index++) {
    Tile * tile = tileset_get(set, index); 
    tile_update(tile, dt);
  }  
} 


/** Draw a tile to the current active drawing target at the
given coordinates */
void tile_draw(Tile * tile, int x, int y) {
  Tileset * set = tile->set;
  Image * sheet = set->sheet;
  float dx      = (float) x;
  float dy      = (float) y; 
  float sx      = (float) tile->now.x;
  float sy      = (float) tile->now.y;
  float sw      = (float) TILE_W;
  float sh      = (float) TILE_H;
  // printf("%f %f\n", sx, sy);
  al_draw_bitmap_region(sheet, sx, sy, sw, sh, dx, dy, 0);
  // al_draw_bitmap(sheet, dx, dy, 0);
}

/** Tile's index. Returns -1 if tile is NULL; */
int tile_index(Tile * tile) { 
  if (!tile) return -1;
  return tile->index;
}

/**  Information about the tile's properties. Return -1 if tile is NULL. */
int tile_kind(Tile * tile) { 
  if (!tile) return -1;
  return tile->kind;
}





