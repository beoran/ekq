
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

/* A simple 3D model. May only have a single texture. 
 * Since the model uses a custom vertex declaration, it's neccesary for an
 * active display to have been opened before creating or loading a model.
 */
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
  Rot3d             angular_speed;
    
  ALLEGRO_TRANSFORM     transform;
  ALLEGRO_VERTEX_DECL * vdecl;
};


  /*
   * For drawing a d3 model or object combined with a camera
   * transform this is what to do:
  al_identity_transform(&trans); 
  al_scale_transform_3d(&trans, 0.3, 2, 1);
  al_rotate_transform_3d(&trans, 0, 1, 0, 2);
  al_translate_transform_3d(&trans, 1, 1, -4);
  al_compose_transform(&trans, &camera);
  al_use_transform(&trans);
  draw_wall(0, 0, 0, 2, 2, wcolors, walltex);
  al_use_transform(&camera);
  */


/* Makes an alleggro vertex declaratio, that works with ALLEGRO_VERTEX, 
 * but the texture coordinates are not per-pixel but normalized. */
ALLEGRO_VERTEX_DECL * model_make_vertex_decl(void) {
  ALLEGRO_VERTEX_ELEMENT elems[] = {
    {ALLEGRO_PRIM_POSITION  , ALLEGRO_PRIM_FLOAT_3, offsetof(ALLEGRO_VERTEX, x)},
    {ALLEGRO_PRIM_TEX_COORD , ALLEGRO_PRIM_FLOAT_2, offsetof(ALLEGRO_VERTEX, u)},
    {ALLEGRO_PRIM_COLOR_ATTR, 0                   , offsetof(ALLEGRO_VERTEX, color)},
    {0, 0, 0}
  };
  return al_create_vertex_decl(elems, sizeof(ALLEGRO_VERTEX));
}


void model_make_transform(ALLEGRO_TRANSFORM * trans, 
    Vec3d * translate, Rot3d * rotate, Vec3d * scale) {

  al_identity_transform(trans); 
  al_scale_transform_3d(trans, scale->x, scale->y, scale->z);
  al_rotate_transform_3d(trans, 0, 0, 1, rotate->rz);
  al_rotate_transform_3d(trans, 0, 1, 0, rotate->ry);
  al_rotate_transform_3d(trans, 1, 0, 0, rotate->rx);
  al_translate_transform_3d(trans, translate->x, translate->y, translate->z);
}


void model_update_transform(Model * me) {
  model_make_transform(&me->transform, &me->position, &me->rotation, &me->size);
}


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
  al_destroy_vertex_decl(me->vdecl);
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
  me->size      = vec3d(1.0, 1.0, 1.0);
  me->sverts    = MODEL_VERTEX_SPACE;
  me->sfaces    = MODEL_FACE_SPACE;
  me->texture   = NULL;
  me->vertices  = calloc(MODEL_VERTEX_SPACE, sizeof(*me->vertices));
  me->faces     = calloc(MODEL_FACE_SPACE  , sizeof(*me->faces));  
  me->objfile   = NULL;
  me->vdecl     = model_make_vertex_decl(); 
  
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
  
  // LOG_DEBUG("Added triangle %d\n", me->nfaces / 3);
  
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
    // model_convert_from_objfile(me , me->objfile);
    return texture;
}


int model_set_scale(Model * me, float sx, float sy, float sz) {
  me->size.x = sx;
  me->size.y = sy;
  me->size.z = sz;
  model_update_transform(me);
  return 0;
}

int model_set_position(Model * me, float x, float y, float z) {
  me->position.x = x;
  me->position.y = y;
  me->position.z = z;
  model_update_transform(me);
  return 0;
}

int model_set_speed(Model * me, float vx, float vy, float vz) {
  me->speed.x = vx;
  me->speed.y = vy;
  me->speed.z = vz;
  model_update_transform(me);
  return 0;
}

int model_set_rotation(Model * me, float rx, float ry, float rz) {
  me->rotation.rx = rx;
  me->rotation.ry = ry;
  me->rotation.rz = rz;
  model_update_transform(me);
  return 0;
}

int model_set_angular_speed(Model * me, float vx, float vy, float vz) {
  me->angular_speed.rx = vx;
  me->angular_speed.ry = vy;
  me->angular_speed.rz = vz;
  model_update_transform(me);
  return 0;
}


void model_update(Model * me, double dt) {
  /* Apply the speed ... */
  Vec3d aid = vec3d_add(me->position, vec3d_mul(me->speed, dt));
  me->position = aid;
  /* ... and the angular speed. */
  Rot3d rot = rot3d_add(me->rotation, rot3d_mul(me->angular_speed, dt));
  me->rotation = rot;
  model_update_transform(me);
}

void model_draw(Model * me) {
  int index;
  ALLEGRO_TRANSFORM camera, model;
  const ALLEGRO_TRANSFORM * now;
  /* Get the current transform and use it as the camera transform.  */
  now = al_get_current_transform();
  al_copy_transform(&camera, now);  
  /* Copy the model's transform, and compose it with the camera transform. */
  al_copy_transform(&model, &me->transform); 
  al_compose_transform(&model, &camera);
  al_use_transform(&model);
  /* Disable the depth test */
  // al_set_render_state(ALLEGRO_DEPTH_TEST, 0);
  
  al_draw_indexed_prim(me->vertices, me->vdecl, me->texture, me->faces, me->nfaces, ALLEGRO_PRIM_TRIANGLE_LIST);
  
  
  /* Re-enable the depth test */
  // al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
  
  /** Restore the camera transform. */
  al_use_transform(&camera);

}


/* Fills in or updates the data of the model with that of the given obj file.
 * The model's texture is used to generate correct UV coordinates for drawing. 
 * The model's points and textures are emptied for this operation.
 */
Model * model_convert_from_objfile(Model * me , ObjFile * objfile) {
  int index, pindex;

  if (!objfile) return NULL;
  if (!model_remove_all_faces(me))    return NULL;
  if (!model_remove_all_vertices(me)) return NULL;
  
  for (index = 1; index <= objfile_get_face_count(objfile); index++) {
    int my_idx[16];
    ObjFace * face = objfile_get_face(objfile, index);
    // First store the face's points into the points of the model
    for (pindex = 0; (pindex < face->n_points) && (pindex < 16); pindex++) {
      ObjFacePoint * point = objface_get_point(face, pindex);
      if (!point) { 
        LOG_WARNING("Misssing point? %d\n", pindex);
        continue;
      }
      
      Vec3d * v            = objfile_get_vertex(objfile, point->i_v);
      Vec3d * vt           = objfile_get_uv(objfile, point->i_vt);
      if (v) {
        my_idx[pindex] = model_add_vertex(me, v->x, v->y, v->z);
      } else {
        LOG_WARNING("Unknown vertex index %d\n", point->i_v);
      }
      
      if (vt) {
        model_set_uv(me, my_idx[pindex], vt->x, -vt->y);
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
      // the quads aren't drawn fro the latter half, why?... 
      // If one of the two sets of tris is added it seems to work (apart from the holes).
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
          filename, me->nverts, me->nfaces / 3);
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
  






