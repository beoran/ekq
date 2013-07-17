#include "eruta.h"
#include "mem.h"
#include "tile.h"
#include "camera.h"
#include "tilepane.h"

#include "pointergrid.h"

/* Tile blending direction constants. */
#define TILE_BLEND_NORTHWEST            0 
#define TILE_BLEND_NORTH                1
#define TILE_BLEND_NORTHEAST            2
#define TILE_BLEND_WEST                 3
#define TILE_BLEND_EAST                 4
#define TILE_BLEND_SOUTHWEST            5
#define TILE_BLEND_SOUTH                6
#define TILE_BLEND_SOUTHEAST            7
#define TILE_BLEND_DIRECTION_MAX        8

#define TILE_BLEND_CORNER               0
#define TILE_BLEND_SIDE                 1
#define TILE_BLEND_SHAPE_MAX            2


/* Tile blending types. */
#define TILE_BLEND_SHARP                0
#define TILE_BLEND_GRADUAL              1
#define TILE_BLEND_FUZZY                2
#define TILE_BLEND_FUZZY_GRADUAL        3
#define TILE_BLEND_TYPE_MAX             4

/*
 * A Tileblend is an auxilliary structure that helps to implement automatic 
 * blending around a certain tile. 
 * 
 */
typedef struct Tileblend_ Tileblend;

struct Tileblend_ {
  Tile * blends[TILE_BLEND_DIRECTION_MAX];
};

/** Allocates a tileblend structure. */
Tileblend * tileblend_alloc() {
  return STRUCT_ALLOC(Tileblend);
}

/** Initializes a tileblend */
Tileblend * tileblend_init(Tileblend * self) {
  int index;
  if(!self) return NULL;
  for(index = 0; index < TILE_BLEND_TYPE_MAX; index++) { 
    self->blends[index] = NULL;
  }
  return self;
}

/* Allocates and initializes a tileblend structure. */
Tileblend * tileblend_new() {
  Tileblend * self;
  self = tileblend_alloc();
  return tileblend_init(self);
}

/* Frees a tileblend structure */
void * tileblend_free(Tileblend * self) {
  if(!self) return NULL;
  return mem_free(self);
}

void * tileblend_destructor(void * self) {
  return tileblend_free(self);
}



#define TILE_BLEND_BITMAPS_MAX 255 

/* A cache of generated generic blending masks. */
static Image * tile_blend_masks[TILE_BLEND_TYPE_MAX][TILE_BLEND_SHAPE_MAX];

/* How the mask should be applied, i. e. flipped. */
static int tile_blend_flips[TILE_BLEND_DIRECTION_MAX] = {
  0,
  0,
  ALLEGRO_FLIP_VERTICAL,
  0,
  ALLEGRO_FLIP_VERTICAL,
  ALLEGRO_FLIP_HORIZONTAL,
  ALLEGRO_FLIP_HORIZONTAL,
  ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL
};

/* Which "side" mask should be used mask should be used. */
static int tile_blend_sides[TILE_BLEND_DIRECTION_MAX] = {
  TILE_BLEND_CORNER,
  TILE_BLEND_SIDE  ,
  TILE_BLEND_CORNER,
  TILE_BLEND_SIDE  ,
  TILE_BLEND_SIDE  ,
  TILE_BLEND_CORNER,
  TILE_BLEND_SIDE  ,
  TILE_BLEND_CORNER
};

/* 
 
 In the simple case where there are only 2 different kinds of tiles, 
 then for a tile somewhere in the map, that tile has 8 neighbours, and thus 
 there are 8**2 or 256 different possible layouts of these neighbours around that 
 single tile, and 256 different blends would be needed.
 
 Of course, this is a staggering amount for hand drawn graphics, and even for 
 automatically generated mask images, this takes up a hefty chunk of memory,
 so some kind of simplification is needed. 
 
 The first simplification is the use of the tile's blend value as a blend *priority*
 value. The tile with the lower blend priority will be blended with the 
 tile with the higher blend priority blending over it. Tiles with equal blend 
 priority will simply not blend. This reduces the possibilities because in 
 cases where 3 or more tyle types must blend, without priorities, 
 the number of possible blends would become even larger. 
 
 Let's condsider the possibilities takng symmetry in account. If only 2 tiles 
 that will blend are adjacent, there are 8 psossibilities. Howevere there ar 4 
 symmetric rotations of the 2 cases of the 2 tiles being either side-to-side 
 or corner to corner. In case of side-to side, the blend should go rougmly like 
 this: (scaled down to 8x8
 
 ..........OOOOOOOO     ................OO
 ..........OOOOOOOO     ..............OOOO
 ..........OOOOOOOO     ............OOOOOO
 ..........OOOOOOOO     ............OOOOOO
 ..........OOOOOOOO =>  ............OOOOOO
 ..........OOOOOOOO     ............OOOOOO
 ..........OOOOOOOO     ...........OOOOOOO
 ..........OOOOOOOO     ..........OOOOOOOO
 
 And corner to corner:
 
           OOOOOOOO               OOOOOOOO
           OOOOOOOO               OOOOOOOO
           OOOOOOOO               OOOOOOOO
           OOOOOOOO               OOOOOOOO
           OOOOOOOO =>            OOOOOOOO
           OOOOOOOO               OOOOOOOO
           OOOOOOOO               .OOOOOOO
           OOOOOOOO               ..OOOOOO
 ..........             ..........        
 ..........             ..........        
 ..........             ..........        
 ..........             ..........        
 ..........         =>  ..........        
 ..........             ..........        
 ..........             ..........        
 ..........             ..........        
 
 If the masks are judiciouly chosen, and all align correctly, 
 then it will suffice to have just 2 mask images which get rotated as needed,
 one mask for the side to side, and one for corner to corner. 
 Each of the masks follows a strict pattern, that is, the side by side 
 has a \__/ - like shape were the depth of the overlap is at most 1/4th of the 
 tile height. (Or is it 1/3, let's check it out). 
 
 Then, for everyoverlapping tile 
 
 
 
 
*/



/**
* A Tilepane is a pane or layer of tiles for use in a tile map or tiled
* background. A Tilepane consists of individual tiles from the same
* tile set. Different panes may have different tile sets.
*/
struct Tilepane_ {
  Tileset     * set;
  PointerGrid * tiles;
  int           gridwide; // width of the tile map in grid points (tiles)
  int           gridhigh; // height of the tile map in grid points (tiles)
  int           realwide; // width of whole tile map in pixels
  int           realhigh; // height of whole tile map in pixels
  PointerGrid * blends;   // Blends for the layer.
};



/** Returns the tile blend helper structure for the given tile, if any.  */
Tileblend * tilepane_blend_xy(Tilepane * self, int x, int y) {
   void      * pblend= NULL;
   if(pointergrid_get(self->blends, x, y, &pblend)) return NULL;
   return (Tileblend *) pblend;
}

/** Sets the tile blend helper structure to the given pointer.  */
Tileblend * tilepane_blend_xy_(Tilepane * self, int x, int y, Tileblend * blend) {
   void      * pblend = blend;
   if(pointergrid_put(self->blends, x, y, blend)) return NULL;
   return (Tileblend *) pblend;
}


/** 
* Cleans up a pane by freeing the memory references it holds internally,
* but does not free the pane itself.
*/
Tilepane * tilepane_done(Tilepane * pane) {
  int index;
  if (!pane) return NULL;
  pointergrid_free(pane->tiles);
  
  pointergrid_nullall(pane->blends, tileblend_destructor);
  pointergrid_free(pane->blends); 
  
  // Size is now zero.
  pane->gridhigh = 0;
  pane->gridwide = 0;
  return pane;
}

/** Deallocates a Tilepane. */
Tilepane * tilepane_free(Tilepane * pane) {
  tilepane_done(pane);
  mem_free(pane);
  return NULL;
}


/** Initializes a tile pane.
* The pane will not clean up the tile set itself!
*/
Tilepane * tilepane_init(Tilepane * pane, Tileset * set,
                           int gridwide, int gridhigh) {
  int x, y, index;
  if (!pane) { perror("Could not allocate pane "); return NULL; }
  tilepane_tileset_(pane, set);
  pane->gridwide   = gridwide;
  pane->gridhigh   = gridhigh;
  pane->realwide   = TILE_W * pane->gridwide;
  pane->realhigh   = TILE_H * pane->gridhigh;
  // declare background, etc, empty
  
  // Precalculate dimensions...  
  // And allocate space for the tiles and tile indices.
  
  pane->tiles    = pointergrid_new(gridwide, gridhigh);
  if(!pane->tiles) {
    perror("Could not allocate tiles matrix ");
    return NULL;
  }
  
  pane->blends    = pointergrid_new(gridwide, gridhigh);
  if(!pane->blends) {
    perror("Could not allocate blends matrix ");
    return NULL;
  }
  // null the tiles and blends
  pointergrid_nullall(pane->tiles, NULL);
  pointergrid_nullall(pane->blends, NULL);

  return pane;
}

/** Makes new gy pane. */
Tilepane * tilepane_new(Tileset * set, int gridwide, int gridhigh) {
  Tilepane * pane = STRUCT_ALLOC(Tilepane);
  return tilepane_init(pane, set, gridwide, gridhigh);
}


/** Returns the width of the pane in grid units. Returns -1 on error. */
int tilepane_gridwide(Tilepane * pane) {
  if(!pane) return -1;
  return pane->gridwide;
}

/** Returns the height of the pane in grid units. Returns -1 on error. */
int tilepane_gridhigh(Tilepane * pane) {
  if(!pane) return -1;
  return pane->gridhigh;
}

/** Returns the width of the pane's tiles in pixels. Returns -1 on error. */
int tilepane_tilewide(Tilepane * pane) {
  if(!pane) return -1;
  return TILE_W;
}

/** Returns the height of the pane's tiles in pixels. Returns -1 on error. */
int tilepane_tilehigh(Tilepane * pane) {
  if(!pane) return -1;
  return TILE_H;
}

/** Returns the width of the pane in pixels. Returns -1 on error. */
int tilepane_wide(Tilepane * pane) {
  if(!pane) return -1;
  return pane->realwide;
}

/** Returns the height of the pane in pixels. Returns -1 on error. */
int tilepane_high(Tilepane * pane) {
  if(!pane) return -1;
  return pane->realhigh;
}

/** Returns TRUE if the given gridx and gridy are outside the grid
* Returns FALSE if inside the grid.
*/
int tilepane_outsidegrid(Tilepane * pane, int gridx, int gridy) {
  if (!pane) return TRUE;
  if ((gridx < 0) || (gridy < 0)) return TRUE;
  if ((gridx >= pane->gridwide) || (gridx >= pane->gridhigh)) return TRUE;
  return FALSE;
}

/** Sets the tile at the given location to the given Tile pointer,
* which may be NULL. Returns the tile thus set, or NULL on error.
*/
Tile * tilepane_set(Tilepane * pane,
                          int gridx, int gridy, Tile * tile) {
  if (tilepane_outsidegrid(pane, gridx, gridy)) return NULL;
  pointergrid_put(pane->tiles, gridx, gridy, tile);
  return tile;
}  

/** Returns the tile in the pane's grid at the given grid coordinates,
* returns NULL if the fcoordinates are out of bounds or if it was an empty tile.
*/
Tile * tilepane_get(Tilepane * pane, int gridx, int gridy) {
  if (pointergrid_outofrange(pane->tiles, gridx, gridy)) return NULL;
  return pointergrid_getraw(pane->tiles, gridx, gridy);
}  

/** Returns the ile index in the pane's grid at the given grid coordinates,
* Returns -1  on error or if no tile is there, and a nonnegative index if a 
* tile was found.
*/
int tilepane_getindex(Tilepane * pane, int gridx, int gridy) {
  Tile * tile = tilepane_get(pane, gridx, gridy);
  return tile_index(tile);
}


/** Sets the tile in the given rectangle  to the given Tile pointer,
* which may be NULL. Returns the tuile set, or NULL on error.
*/
Tile * tilepane_rect(Tilepane * pane,
                     int gridx, int gridy, int gridw, int gridh,
                     Tile * tile) {
  int ii, jj;
  for (jj = gridy; jj < (gridy + gridh) ; jj++){
    for (ii = gridx; ii < (gridx + gridw) ; ii++) {
      tilepane_set(pane, ii, jj, tile);
    }
  }
  return tile;
}

/** Fills the while tile pane with the given tile.
Returns tile if ok, or NULL on error. */
Tile * tilepane_fill(Tilepane * pane, Tile * tile) {
  int ww = tilepane_gridwide(pane);
  int hh = tilepane_gridhigh(pane);
  return tilepane_rect(pane, 0, 0, ww, hh, tile);
}


/* Draws the blends of the tile pane for the given tile. */
void tilepane_draw_tile_blend
(Tilepane * self, int tx, int ty, int drawx, int drawy, Tile * tile) {
  Image * mask;
  Tile * other;
  Tileblend * blend;
  // Color color;
  int flags, side;
  int index;
  blend = tilepane_blend_xy(self, tx, ty);
  if(!blend) return;
  // al_draw_bitmap(tile_blend_masks[0][0], 20, 60, 0);
  // al_draw_bitmap(tile_blend_masks[0][1], 60, 60, 0);
  
  /*color = al_map_rgb(0, 20, 255);
  al_draw_filled_circle(30, 50, 4, color);
  puts("OK!");*/

  for (index= 0 ; index < TILE_BLEND_DIRECTION_MAX; index ++) {
    other = blend->blends[index];
    if(!other) { continue; } 
    // color = al_map_rgba(20 * index, 20 * index, 20 * index, 255);
    side  = tile_blend_sides[index];
    mask  = tile_blend_masks[0][side];
    flags = tile_blend_flips[index];
    // printf("Blend %d %d %p %p at %d %d & %d %d.\n", side, flags, other, mask, tx, ty, drawx, drawy);
    tile_draw_masked(other, drawx, drawy, mask, flags);
    // al_draw_filled_rectangle(fx, fy, fx+4, fy+4, color);
  }
} 


// Since tile size is 32, shifts can be used in stead of multiplications.
#define TIMES_TILEWIDE(V) ((V) << 5)
#define TIMES_TILEHIGH(V) ((V) << 5)

/** Draws the blends of the tile tile pane, with x and y as the top left corner,
* to the current active display.  x and y may be negative.
* This is a repeat of the function tilepane_draw below,
* because that function makes use of the bitmap holding feature to speed up drawing 
* the layer, which means that blending as is needed for drawing the tile blends is 
* not available. 
* This function repeats exactly the same deep loop, but without bitmap 
* drawing being held, just to draw the blends. 
*/
void tilepane_draw_blends(Tilepane * pane, Camera * camera) {
  // Copy everything to the stack since that should be faster than always
  // referring to pointers.
  int gridwide    = pane->gridwide;
  int gridhigh    = pane->gridhigh;
  int tilewide    = TILE_W;
  int tilehigh    = TILE_H;
  int x           = (int) camera_at_x(camera);
  int y           = (int) camera_at_y(camera);  
  int txstart     = x / tilewide;
  int tystart     = y / tilehigh;
  int xtilestop   = (camera_w(camera) / tilewide) + 1;
  int ytilestop   = (camera_h(camera) / tilehigh) + 1;
  int txstop      = xtilestop + txstart;
  int tystop      = ytilestop + tystart;
  int drawx       = 0;
  int drawy       = 0;
  int ty_index    = 0;
  int tx_index    = 0;
  int realwide    = pane->realwide;
  int realhigh    = pane->realhigh;
  Tile ** row     = NULL;
  Tile * tile     = NULL;
  Tileblend * blend= NULL;
  Color color     = al_map_rgb(200,200,200);
  Color color2    = al_map_rgb(0,50,0);
    
  if (txstart >= realwide) return;
  if (tystart >= realhigh) return;
  al_hold_bitmap_drawing(TRUE);
  txstart         = (txstart < 0) ? 0 : txstart;
  tystart         = (tystart < 0) ? 0 : tystart;
  txstop          = (txstop > gridwide) ? gridwide : txstop;
  tystop          = (tystop > gridhigh) ? gridhigh : tystop;  
  drawy           = -y + TIMES_TILEHIGH(tystart-1);
  for (ty_index = tystart; ty_index < tystop ; ty_index++) {
    drawy        += tilehigh;
    drawx         = -x + TIMES_TILEWIDE(txstart-1);
    row           = (Tile **) pointergrid_rowraw(pane->tiles, ty_index);
    if(!row) continue;
    for(tx_index = txstart; tx_index < txstop ; tx_index++) { 
      drawx      += tilewide;
      tile        = row[tx_index];
      if(tile) {
        blend = tilepane_blend_xy(pane, tx_index, ty_index);
        if (blend) { 
          tilepane_draw_tile_blend(pane, tx_index, ty_index, drawx, drawy, tile);
        }
      } 
    }
  }
}

/** Draws the tile pane, with x and y as the top left corner,
* to the current active display  X and y may be negative.
*/
void tilepane_draw(Tilepane * pane, Camera * camera) {
  // Copy everything to the stack since that should be faster than always
  // referring to pointers.
  int gridwide    = pane->gridwide;
  int gridhigh    = pane->gridhigh;
  int tilewide    = TILE_W;
  int tilehigh    = TILE_H;
  int x           = (int) camera_at_x(camera);
  int y           = (int) camera_at_y(camera);  
  int txstart     = x / tilewide;
  int tystart     = y / tilehigh;
  int xtilestop   = (camera_w(camera) / tilewide) + 1;
  int ytilestop   = (camera_h(camera) / tilehigh) + 1;
  int txstop      = xtilestop + txstart;
  int tystop      = ytilestop + tystart;
  int drawx       = 0;
  int drawy       = 0;
  int ty_index    = 0;
  int tx_index    = 0;
  int realwide    = pane->realwide;
  int realhigh    = pane->realhigh;
  Tile ** row     = NULL;
  Tile * tile     = NULL;
  Tileblend * blend= NULL;
  Color color     = al_map_rgb(200,200,200);
  Color color2    = al_map_rgb(0,50,0);
    
  if (txstart >= realwide) return;
  if (tystart >= realhigh) return;
  // hold drawing since all tile draws come from the same tile sheet
  al_hold_bitmap_drawing(TRUE);
  txstart         = (txstart < 0) ? 0 : txstart;
  tystart         = (tystart < 0) ? 0 : tystart;
  txstop          = (txstop > gridwide) ? gridwide : txstop;
  tystop          = (tystop > gridhigh) ? gridhigh : tystop;  
  drawy           = -y + TIMES_TILEHIGH(tystart-1);
  for (ty_index = tystart; ty_index < tystop ; ty_index++) {
    drawy        += tilehigh;
    drawx         = -x + TIMES_TILEWIDE(txstart-1);
    row           = (Tile **) pointergrid_rowraw(pane->tiles, ty_index);
    if(!row) continue;
    for(tx_index = txstart; tx_index < txstop ; tx_index++) { 
      drawx      += tilewide;
      tile        = row[tx_index];
      // Null tile will not be drawn by tile_draw
      /* tile_draw(tile, drawx, drawy); */
      if(tile) {
        tile_draw(tile, drawx, drawy);
      } 
    }
  }
  // Let go of hold 
  al_hold_bitmap_drawing(FALSE);  
  
  // Now, after the hold are released , draw the blends. 
  tilepane_draw_blends(pane, camera);
  
}







/** Updates the tile pane. Curently animates the pane. */
void tilepane_update(Tilepane * pane, double dt) {
  
  
}





/** Sets this Tilepane's tile set. */
Tileset * tilepane_tileset_(Tilepane * pane, Tileset * set) {
  if (!pane) return NULL;
  return pane->set = set;
}

/** Gets this Tilepane's tile set. */
Tileset * tilepane_tileset(Tilepane * pane) {
  if (!pane) return NULL;
  return pane->set;
}

/** Gets a tile from a the tilepane's tile set by it's tile id. **/
Tile * tilepane_getfromset(Tilepane * pane, int index) {  
  Tileset * set = tilepane_tileset(pane);
  if (!set) return NULL;
  if (index < 0) return NULL;
  return tileset_get(set, index);
}

/** Sets the tile at the given location to tile at index index in the
* Tilepane's Tileset, in index < 0, sets NULL; Returns the Tile * object
* thus set.
*/
Tile* tilepane_setindex(Tilepane * pane,
                             int gridx, int gridy, int index) {
  Tile * tile = tilepane_getfromset(pane, index);
  return tilepane_set(pane, gridx, gridy, tile);
}


/** Sets the tile in the given rectangle to the given Tile index,
* Returns the tile thus set, or NULL on error or if the index was negative.
*/
Tile * tilepane_rectindex(Tilepane * pane,
                         int gridx, int gridy, int gridw, int gridh,
                         int index) {
  Tile * tile = tilepane_getfromset(pane, index);
  return tilepane_rect(pane, gridx, gridy, gridw, gridh, tile);
}

/** Fills the while tile pane with the given tile index */
Tile * tilepane_fillindex(Tilepane * pane, int index) {
  Tile * tile = tilepane_getfromset(pane, index);
  return tilepane_fill(pane, tile);
}



struct TileblendOffset {
  int tx;
  int ty;
};

static struct TileblendOffset tile_blend_offsets[] = {
  {-1, -1}, /* TILE_BLEND_NORTHWEST 0 */ 
  { 0, -1}, /* TILE_BLEND_NORTH     1 */
  { 1, -1}, /* TILE_BLEND_NORTHEAST 2 */
  {-1,  0}, /* TILE_BLEND_WEST      3 */
  { 1,  0}, /* TILE_BLEND_EAST      4 */
  {-1,  0}, /* TILE_BLEND_SOUTHWEST 5 */
  {-1,  1}, /* TILE_BLEND_SOUTHWEST 5 */
  { 0,  1}, /* #TILE_BLEND_SOUTH    6 */
  { 1,  1}, /* TILE_BLEND_SOUTHEAST 7 */
};


/** Sets up automatic blending for this paneand this tile in the pane. */
bool
tilepane_init_blend_tile(Tilepane * self, int index, int x, int y, Tile * tile) {
  int bindex, yn, xn;
  void      * aid   = NULL;
  Tileblend * blend = NULL;
  Tileblend * blend2= NULL;
  Tile      * aidtile;
  int tileprio, aidprio;
  blend = tilepane_blend_xy(self, x, y);
  /** Reuse old blend if set, otherwise make new one. */
  if (blend) { 
    tileblend_init(blend);
  } else {
    blend = tileblend_new();
    tilepane_blend_xy_(self, x, y, blend);
  }

  tileprio = tile_blend(tile);
  /* printf("Blend tile at %d %d %d\n", index, x, y); */



  /* Look for the tiles around self. */
  for (bindex = 0; bindex < TILE_BLEND_DIRECTION_MAX ; bindex++) { 
    struct TileblendOffset offset = tile_blend_offsets[bindex];
    /* Begin by setting NULL to mean "no blend tile there" */
    blend->blends[bindex] = NULL;
    yn      = y + offset.ty;
    xn      = x + offset.tx;
    aidtile = tilepane_get(self, xn, yn);
    /* No tile there, no blend. */
    if (!aidtile) continue;
    aidprio = tile_blend(aidtile);
    /* Only blend if blend priority of other is higher. */
    if (aidprio <= tileprio) continue;
    blend->blends[bindex] = aidtile;
   
    printf("Tile at %d %d %d will blend with tile at %d %d\n", index, x, y, xn, yn);
    
  }
 
  return TRUE;  
} 

/* For the sake of loading the masks. */
ALLEGRO_BITMAP * fifi_load_bitmap(const char * vpath); 

#define MASK_SIDE_W     16
#define MASK_SIDE_H     16
#define MASK_CORNER_W   16
#define MASK_CORNER_H   16
#define MASK_W 8
#define MASK_H 8

/* Sets up the tile blend masks if needed. */
bool tilepane_init_masks() {
  Image * image1, * image2, *target;
  Color solid, transparent, color; 
  /* Already initialized. */
  if (tile_blend_masks[0][0]) return TRUE;
  /* Load the masks */
  image1 = fifi_load_bitmap("/image/masks/corner_mask.png");
  image1 = fifi_load_bitmap("/image/masks/corner_mask.png");
  
  
  /* */
  image1 = al_create_bitmap(TILE_W, TILE_H);
  image2 = al_create_bitmap(TILE_W, TILE_H);
  /* Make top image like this: image like this : \_/ */
  if (!(image1 && image2)) {
    al_destroy_bitmap(image1);
    al_destroy_bitmap(image2);
    return false;
  } 
  /* Make corner image like this : / */
  solid         = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
  // transparent   = al_map_rgba_f(1.0, 1.0, 1.0, 0.0);
  transparent   = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
  tile_blend_masks[0][TILE_BLEND_CORNER] = image1;
  tile_blend_masks[0][TILE_BLEND_SIDE]   = image2;
  
  target= al_get_target_bitmap();
  al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
  /* Draw corner mask. */
  al_set_target_bitmap(image1);
  al_clear_to_color(transparent);
  al_draw_filled_triangle( 0,0,  MASK_CORNER_W,0, 0,MASK_CORNER_H, solid);
  
  /* Draw side mask. like this: \_/ */
  al_set_target_bitmap(image2);
  al_clear_to_color(transparent);
  al_draw_filled_triangle
  ( 0,0,  MASK_SIDE_W, 0, MASK_SIDE_W, MASK_SIDE_H, solid);
  al_draw_filled_triangle
  (TILE_W - MASK_SIDE_W, 
   0, TILE_W, 0,  
   TILE_W - MASK_SIDE_W, MASK_SIDE_H, solid);
  al_draw_filled_rectangle
  (MASK_SIDE_W, 0, TILE_W - MASK_SIDE_W, MASK_SIDE_H, solid);
  
  /* Restore normal Allegro blending and target bitmap (i.e., the display). */
  al_set_target_bitmap(target);
  al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
  return TRUE;
};


/** Sets up automatic blending for this tile pane. */
bool tilepane_init_blend(Tilepane * self, int index) {
  int x, y, w, h;
  if (!self)             return FALSE;
  if (!self->blends)     return FALSE; 
  w = tilepane_gridwide(self);
  h = tilepane_gridhigh(self);
  for (y = 0  ; y < h; y++) { 
    for (x = 0; x < w; x++) {
      Tile * tile = tilepane_get(self, x, y);
      if (!tile) continue;
      if(!tile_blend(tile)) continue;
      tilepane_init_blend_tile(self, index, x, y, tile);
    }
  }
  return tilepane_init_masks();
}



/* Generic blend masks for every tile map.  */






