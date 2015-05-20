/* Integration of extended , more complex resources such as tile maps 
 * into the resor/storage system.
 */


#include "eruta.h"
#include "resor.h"
#include "fifi.h"
#include "xresor.h"


#ifdef COMMENT_

void * xresor_maze_loader(const char * vpath, void * extra_) {  
  Tilemap * map;
  TilemapLoadExtra extra;
  (void) extra_;
  map = fifi_load_vpath(_fifi_load, NULL, vpath);
  return map;
}

int xresor_tilemap_destructor(Resor * resor) {
  Tilemap * map;
  if (!resor) return -1;
  map = resor_other(resor, RESOR_TILEMAP);
  tilemap_free(map);
  return 0;
}


Resor * xresor_load_tilemap(const char * vpath, void * extra) { 
  return resor_load_other(vpath, RESOR_TILEMAP, xresor_tilemap_loader, xresor_tilemap_destructor, extra);
}

#endif








