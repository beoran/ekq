
#include <stdio.h>
#include "eruta.h"
#include "fifi.h"
#include "monolog.h"
#include "model.h"
#include "store.h"
#include "bevec.h"
#include "vec3d.h"
#include "objfile.h"

/* Space to allocated by default, also used as linear increment. */
#define MODEL_VERTEX_SPACE 1024
#define MODEL_FACE_SPACE   1024
#define MODEL_UV_SPACE     1024


/* Notes about the OBJ format:
 * Both texture coordinates and point coordnates are re-used in the f 
 * declarations. This means that the actual model will have more points 
 * that there are v statements, since I draw using a triangle list.
 * Even though the v is the same, if the vt is different then it must be drawn 
 * using a new polygon.
 * 
 */

/* A simple 3D model. May only have a single texture. */
struct Model_ {
  /* Obj file from which the model is loaded. Must be kept 
   * since reassigning the texture means the uv coords have to be recalculated
   * for allegro's sake. */
  ObjFile         * objfile; 
   
  
  ALLEGRO_VERTEX  * vertices;
  int             * faces;
  
  int               sverts;
  int               sfaces;
  int               nverts;
  int               nfaces;  
  
  ALLEGRO_BITMAP  * texture;
  Vec3d             position;
  Vec3d             speed;
  Vec3d             size;
  Rot3d             rotation;
    
  ALLEGRO_TRANSFORM transform;
};

Model * model_alloc() {
  return calloc(sizeof(Model), 1);
}

Model * model_remove_all_vertices(Model * me) {
  if (!me) return NULL;
  free(me->vertices);
  me->vertices = NULL;
  me->nverts   = 0;
  me->sverts   = 0;
  return me;  
}

Model * model_remove_all_faces(Model * me) {
  if (!me) return NULL;
  free(me->faces);
  me->faces   = NULL;
  me->nfaces  = 0;
  me->sfaces  = 0;
  return me;  
}


Model * model_done(Model * me) {  
  if (!me) return NULL;
  model_remove_all_vertices(me);
  model_remove_all_faces(me);
  objfile_free(me->objfile);
  me->objfile   = NULL;  
  return NULL;
}

Model * model_free(Model * me) {
  model_done(me);
  free(me);
  return NULL;
}


Model * model_init(Model * me) {
  if (!me) return NULL;
  
  me->nverts    = 0;
  me->nfaces    = 0;
  me->sverts    = MODEL_VERTEX_SPACE;
  me->sfaces    = MODEL_FACE_SPACE;
  me->texture   = NULL;
  me->vertices  = calloc(MODEL_VERTEX_SPACE, sizeof(*me->vertices));
  me->faces     = calloc(MODEL_FACE_SPACE  , sizeof(*me->faces));  
  me->objfile   = NULL; 
  
  if ((!me->vertices) || (!me->faces)) {
    return model_done(me);
  }
  
  model_set_position(me, 0, 0, 0);
  model_set_rotation(me, 0, 0, 0);
  
  
  al_identity_transform(&me->transform);
  return me;  
}


Model * model_new() {
  return model_init(model_alloc());
}


int model_add_vertex(Model * me, float x, float y, float z) {
  
  if (me->nverts >= me->sverts) {
    int new_size          = me->sverts + MODEL_VERTEX_SPACE;
    ALLEGRO_VERTEX * aid  = realloc(me->vertices, sizeof(*me->vertices) * new_size);
    if (!aid) return -1;    
    me->sverts            = new_size;
    me->vertices          = aid;
  }
  
  me->vertices[me->nverts].x = x;
  me->vertices[me->nverts].y = y;
  me->vertices[me->nverts].z = z;
  me->vertices[me->nverts].u      = 0.0;
  me->vertices[me->nverts].v      = 0.0;
  me->vertices[me->nverts].color  = al_map_rgb(255, 255, 255);
  
  me->nverts++;
  return me->nverts - 1;
}


int model_add_triangle(Model * me, int i1, int i2, int i3) {
  if (me->nfaces >= (me->sfaces-2)) {
    int new_size          = me->sfaces + MODEL_FACE_SPACE;
    int * aid             = realloc(me->faces, sizeof(*me->faces) * new_size);
    if (!aid) return -1;    
    me->sfaces            = new_size;
    me->faces             = aid;
  }
  
  me->faces[me->nfaces + 0] = i1;
  me->faces[me->nfaces + 1] = i2;
  me->faces[me->nfaces + 2] = i3;
  
  me->nfaces               += 3;
  return me->nfaces - 3;  
}


int model_set_rgba(Model * me, int index, int r, int g, int b, int a) {
  if (index < 0) return -1;
  if (index >= me->nverts) return -2;
  me->vertices[index].color = al_map_rgba(r, g, b, a);

  return index;
}


int model_set_uv(Model * me, int index, float u, float v) {
  if (index < 0) return -1;
  if (index >= me->nverts) return -2;
  me->vertices[index].u = u;
  me->vertices[index].v = v;
  
  return index;
}


Model * model_convert_from_objfile(Model * me , ObjFile * objfile);

/* Sets the texture of the model by using a bitmap index from the resource store. 
 * This entails a full scale recalculation of the model from it's object file.
 */
int model_set_texture(Model * me, int texture) {
    me->texture = store_get_bitmap(texture);
    model_convert_from_objfile(me , me->objfile);
    return texture;
}


int model_set_scale(Model * me, float sx, float sy, float sz) {
  me->size.x = sx;
  me->size.y = sy;
  me->size.z = sz;
  return 0;
}

int model_set_position(Model * me, float x, float y, float z) {
  me->position.x = x;
  me->position.y = y;
  me->position.z = z;
  return 0;
}

int model_set_speed(Model * me, float vx, float vy, float vz) {
  me->speed.x = vx;
  me->speed.y = vy;
  me->speed.z = vz;
  return 0;
}

int model_set_rotation(Model * me, float rx, float ry, float rz) {
  me->rotation.rx = rx;
  me->rotation.ry = ry;
  me->rotation.rz = rz;
  return 0;
}

void model_update(Model * me, double dt);

void model_draw(Model * me) {
  ALLEGRO_TRANSFORM save;
  const ALLEGRO_TRANSFORM * now;
  now = al_get_current_transform();
  /* XXX todo: apply and reset model transform. */
  al_copy_transform(&save, now);  
  al_copy_transform(&me->transform, now);
  /**/
  
  /* This almost worked but it's not correct:
   * al_scale_transform_3d(&me->transform, me->size.x, me->size.y, me->size.z); 
   * al_translate_transform_3d(&me->transform, me->position.x * me->size.x, me->position.y * me->size.y, me->position.z * me->size.z);  
   * */
  
  /*  Rotates also don't work, why?
   * al_rotate_transform_3d(&me->transform, 1, 0, 0, me->rotation.rx);
  al_rotate_transform_3d(&me->transform, 0, 1, 0, me->rotation.ry);
  al_rotate_transform_3d(&me->transform, 0, 0, 1, me->rotation.rz);    
  */
  al_translate_transform_3d(&me->transform, me->position.x, me->position.y, me->position.z);  
  /* al_translate_transform_3d(&me->transform, me->position.x, me->position.y, me->position.z);  */
  /* XXX: Scaling also almost works but the object seems to "float"
   * al_scale_transform_3d(&me->transform, me->size.x, me->size.y, me->size.z); 
   */
  
  al_use_transform(&me->transform);
  
  al_draw_indexed_prim(me->vertices, NULL, me->texture, me->faces, me->nverts, ALLEGRO_PRIM_TRIANGLE_LIST);
  // al_identity_transform(&me->transform);
  al_use_transform(&save);

}


/* Fills in or updates the data of the model with that of the given obj file.
 * The model's texture is used to generatecorrect UV coordinates for drawing. 
 * The model's points and textures are emptied for this operation.
 */
Model * model_convert_from_objfile(Model * me , ObjFile * objfile) {
  int index, pindex;

  if (!model_remove_all_faces(me))    return NULL;
  if (!model_remove_all_vertices(me)) return NULL;
  
  for (index = 1; index <= objfile_get_face_count(objfile); index++) {
    int my_idx[16];
    ObjFace * face = objfile_get_face(objfile, index);
    // First store the face's points into the points of the model
    for (pindex = 0; (pindex < face->n_points) && (pindex < 16); pindex++) {
      ObjFacePoint * point = face->points + pindex;
      Vec3d * v            = objfile_get_vertex(objfile, point->i_v);
      Vec3d * vt           = objfile_get_vertex(objfile, point->i_vt);
      if (v) {
        my_idx[pindex] = model_add_vertex(me, v->x, v->y, v->z);
      } else {
        LOG_WARNING("Unknown vertex index %d\n", point->i_v);
      }
      
      if (vt && me->texture) {
        model_set_uv(me, my_idx[pindex], 
          vt->x  * al_get_bitmap_width(me->texture), 
         -vt->y  * al_get_bitmap_height(me->texture)
        );
      }
      
      if (point->mtl) {
        MtlMaterial * mtl = point->mtl;
        model_set_rgba(me, my_idx[pindex], mtl->Kd[0], mtl->Kd[1], mtl->Kd[2], 1.0 - mtl->Tr);
      }  else {
        model_set_rgba(me, my_idx[pindex], 255, 255, 255, 255);
      }    
    }
    
    if (face->n_points == 3) {
      model_add_triangle(me, my_idx[0], my_idx[1], my_idx[2]);      
    } else if (face->n_points == 4) {
      model_add_triangle(me, my_idx[0], my_idx[1], my_idx[2]);
      model_add_triangle(me, my_idx[0], my_idx[2], my_idx[3]);
    } else {
      LOG_WARNING("Only quad or tri faces allowed!\n");
    }
  }
  return me;    
}


/* obj and mtl file parser. */
Model * model_parse_obj_file(Model * me, FILE * file) {
  if (!me) return NULL;
  me->objfile = objfile_load_file(file); 
  if (!me->objfile) return NULL;
  if (!model_convert_from_objfile(me, me->objfile)) return NULL;
  return me;
}



Model * model_load_obj_file(FILE * file) {
  Model * me = model_new();
  if (!model_parse_obj_file(me, file)) {
    return model_free(me);
  }
  return me;
}

Model * model_load_obj_filename(char * filename) {
  Model * me;
  FILE * file = fopen(filename, "rt");
  if (!file) {
    LOG_ERROR("Cannot open obj file %s\n", filename);
    return NULL;
  }
  
  me = model_load_obj_file(file);
  
  if (!me) {
    LOG_ERROR("Parse error or out of memory in obj file %s\n", filename);
  } else {
    LOG_NOTE("Loaded model from %s with %d points and %d tris\n", 
          filename, me->nverts, me->nfaces);
  }
  
  fclose(file);
  return me;
}


/**
* Loads a model with the given vpath
*/
Model * model_load_obj_vpath(char * vpath) {
  return fifi_loadsimple_vpath(
            (FifiSimpleLoader *)model_load_obj_filename, vpath);
}
  






