
#include <stdio.h>
#include "eruta.h"
#include "fifi.h"
#include "monolog.h"
#include "model.h"
#include "store.h"
#include "bevec.h"
#include "vec3d.h"

/* Space to allocated by default, also used as linear increment. */
#define MODEL_VERTEX_SPACE 1024
#define MODEL_FACE_SPACE   1024
#define MODEL_UV_SPACE     1024


/* A simple 3D model. May only have a single texture. */
struct Model_ {
  ALLEGRO_VERTEX  * vertices;
  int             * faces;
  BeVec           * uv; /* This is meant to ease loading obj files, it's otherwise redundant with vertices.*/
  
  int               sverts;
  int               sfaces;
  int               suv;
  int               nverts;
  int               nfaces;  
  int               nuv;
  
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

Model * model_done(Model * me) {  
  free(me->vertices);
  me->vertices = NULL;
  free(me->faces);  
  me->faces    = NULL;  
  free(me->uv);  
  me->uv       = NULL;  
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
  me->uv        = calloc(MODEL_UV_SPACE    , sizeof(*me->uv));  
    
  if ((!me->vertices) || (!me->faces) || (!me->uv)) {
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

int model_add_uv(Model * me, float u, float v) {
  if (me->nuv >= (me->suv)) {
    int new_size          = me->suv + MODEL_UV_SPACE;
    BeVec * aid           = realloc(me->uv, sizeof(*me->uv) * new_size);
    if (!aid) return -1;    
    me->suv               = new_size;
    me->uv                = aid;
  }
  
  me->uv[me->nuv].x = u;
  me->uv[me->nuv].y = v;
  
  me->nuv++;
  return me->nuv - 1;  
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

/* Sets the texture of the model by using a bitmap index from the resource store. */
int model_set_texture(Model * me, int texture) {
    me->texture = store_get_bitmap(texture);
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
  const ALLEGRO_TRANSFORM * save;
  save = al_get_current_transform();
  /* XXX todo: apply and reset model transform. */
  al_copy_transform(&me->transform, save);
  /**/
  al_translate_transform_3d(&me->transform, me->position.x, me->position.y, me->position.z);
  /* al_scale_transform_3d(&me->transform, me->size.x, me->size.y, me->size.z); */
  al_use_transform(&me->transform);
  
  al_draw_indexed_prim(me->vertices, NULL, me->texture, me->faces, me->nverts, ALLEGRO_PRIM_TRIANGLE_LIST);
  // al_identity_transform(&me->transform);
  al_use_transform(save);

}

/* Copies a uv pair from the internal index uvindex to the vertex index vtindex */
int model_uv_copy(Model * me, int vtindex, int uvindex) {
  return model_set_uv(me, vtindex, me->uv[uvindex].x, me->uv[uvindex].y);
}


#define MODEL_OBJ_LINE_SIZE 1024


Model * model_parse_obj_line(Model * me, char * line) {
  float f1 = 0.0, f2 = 0.0, f3 = 0.0;
  float u = 0.0, v = 0.0, w = 0.0;
  char fc = line[0];
  char command[64];
  char name[256] = { '\0' };
  int v1 = 0, v2 = 0, v3 = 0;
  int t1 = 0, t2 = 0, t3 = 0;
  int n1 = 0, n2 = 0, n3 = 0;
  int i = 0;
  
  /* ignore comments */
  if (fc == '#') return me; 
  
  if (sscanf(line, "v %f %f %f", &f1, &f2, &f3) == 3) {
    if (model_add_vertex(me, f1, f2, f3) < 0) return NULL;
    return me;    
  } 
  
  if (sscanf(line, "vt %f %f %f", &u, &v, &w) >= 2) {
    if (model_add_uv(me, u, v) < 0) return NULL;
    return me;    
  }
  
  /* Simple triangle polygon. */
  if (sscanf(line, "f %d %d %d", &v1, &v2, &v3) == 3) {
       if (model_add_triangle(me, v1 - 1, v2 - 1, v3 - 1) < 0) return NULL;
       return me;
  }
    
  /* Textured polygon. */
  if (sscanf(line, "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3) == 6) {       
       if (model_add_triangle(me, v1 - 1, v2 - 1, v3 - 1) < 0) return NULL;
       /* Copy the model's uv data at index t1 to the uv adata of the point v1, etc */
       model_uv_copy(me, v1 - 1, t1 - 1);
       model_uv_copy(me, v2 - 1, t2 - 1);
       model_uv_copy(me, v3 - 1, t3 - 1);
       return me;
  }
      
  /* Ignore smooth shading */
  if (sscanf(line, "s %d", &i) == 1) {
    return me;
  }
  
  /* Ignore normals */
  if (sscanf(line, "vb %f %f %f", &f1, &f2, &f3) == 3) {
    return me;
  }
  
  /* Ignore object names  */
  if (sscanf(line, "o %255s", name) == 1) {
    return me;
  }
  
  /* Ignore group names  */
  if (sscanf(line, "g %255s", name) == 1) {
    return me;
  }
  
  /* Ignore mtllib statements  */
  if (sscanf(line, "mtllib %255s", name) == 1) {
    return me;
  }
  
  /* Ignore usemtl statements  */
  if (sscanf(line, "usemtl %255s", name) == 1) {
    return me;
  }
  
  /* Ignore newmtl statements  */
  if (sscanf(line, "newmtl %255s", name) == 1) {
    return me;
  }
    
  /* Ignore Ka statements  */
  if (sscanf(line, "Ka %f %f %f", &f1, &f2, &f3) == 3) {
    return me;
  }
  
  /* Ignore Ks statements  */
  if (sscanf(line, "Ks %f %f %f", &f1, &f2, &f3) == 3) {
    return me;
  }
  
  /* Ignore Kd statements  */
  if (sscanf(line, "Kd %f %f %f", &f1, &f2, &f3) == 3) {
    return me;
  }
  
  /* Ignore illum statements  */
  if (sscanf(line, "illum %d", &i) == 1) {
    return me;
  }
  
  /* Ignore Ns statements  */
  if (sscanf(line, "Ns %f", &f1) == 1) {
    return me;
  }
  
  /* Ignore d statements  */
  if (sscanf(line, "d %f", &f1) == 1) {
    return me;
  }
  
  /* Ignore Tr statements  */
  if (sscanf(line, "Tr %f", &f1) == 1) {
    return me;
  }
  
  /* Ignore map_Ka statements  */
  if (sscanf(line, "map_Ka %255s", name) == 1) {
    return me;
  }
  
  /* Ignore map_Kd statements  */
  if (sscanf(line, "map_Kd %255s", name) == 1) {
    return me;
  }
  
  /* Ignore map_Kn statements  */
  if (sscanf(line, "map_Kn %255s", name) == 1) {
    return me;
  }
    
  
  
  // Ignore anything else for now  
  /* 
   * LOG_ERROR("Unknown instruction >%c< in obj/mtl file", fc); 
   * return NULL
   */
    
  return me;
}

/* obj and mtl file parser. */
Model * model_parse_obj_file(Model * me, FILE * file) {
  /* It's reasonable to use a static buffer since line sizes shoun't get THAT long for an obj file. */
  char buffer[MODEL_OBJ_LINE_SIZE];
  
  while (!feof(file)) {
    char * res = fgets(buffer, sizeof(buffer), file); 
    if (!res) {
      if (ferror(file)) {
        LOG_ERROR("Read error when reading in model file.");
        return NULL;
      } else {
        return me;
      }
    }
    if (!model_parse_obj_line(me, buffer)) { 
      return NULL;
    }
  }
  return me;
}



Model * model_load_obj_file(FILE * file) {
  Model * me = model_new();
  return model_parse_obj_file(me, file);
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
  






