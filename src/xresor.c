/* Integration of extended , more complex resources such as tile maps 
 * into the resor/storage system.
 */


#include "eruta.h"
#include "resor.h"
#include "fifi.h"
#include "xresor.h"
#include "model.h"
#include "store.h"

void * xresor_model_loader(const char * vpath, void * extra_) {  
  Model * model;
  (void) extra_;
  model = model_load_obj_vpath((char *)vpath); 
  return model;
}

int xresor_model_destructor(Resor * resor) {
  Model * model;
  if (!resor) return -1;
  model = resor_other(resor, RESOR_MODEL);
  model_free(model);
  return 0;
}

Resor * xresor_load_model(const char * vpath, void * extra) { 
  return resor_load_other(vpath, RESOR_MODEL, xresor_model_loader, xresor_model_destructor, extra);
}

/* Loads a 3d model and puts it in the storage. */
Resor * 
store_load_model(int index, const char * vpath) {
  return store_put(index, xresor_load_model(vpath, NULL));
}

/* Fetches a 3D model from storage. */
Resor * store_get_model(int index) {
  return store_get_other(index, RESOR_MODEL);
}




