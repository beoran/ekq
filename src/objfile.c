
#include <string.h>
#include <stdarg.h>


#include "eruta.h"
#include "objfile.h"
#include "monolog.h"


MtlMaterial * mtlmaterial_init_empty(MtlMaterial * me) {
  float Ka[3] = { 1.0, 1.0, 1.0 };
  float Ks[3] = { 1.0, 1.0, 1.0 };
  float Kd[3] = { 1.0, 1.0, 1.0 };
  if (!me) return NULL;

  
  me->name = me->map_Ka = me->map_Kd = me->map_Kn = NULL;
  memcpy(me->Ka, Ka, sizeof(me->Ka));
  memcpy(me->Ks, Ks, sizeof(me->Ks));
  memcpy(me->Kd, Kd, sizeof(me->Kd));
  me->Ns   = 1.0;
  me->Tr   = 0.0;
  me->illum= 1;
  return me;
}

MtlMaterial * mtlmaterial_done(MtlMaterial * me) {
  if (!me) return NULL;
  free(me->name);
  free(me->map_Ka);
  free(me->map_Kd); 
  free(me->map_Kn); 
  me->name = me->map_Ka = me->map_Kd = me->map_Kn = NULL;
  return me; 
}

/* strdup isn't ANSI C, just posix... :p so we need our own version.*/
char *objfile_strdup(const char *str) {
  if (!str) return NULL;
    char * res = malloc(strlen(str) + 1);
    if(res) { strcpy(res, str); }
    return res;
}


MtlMaterial * mtlmaterial_copy(MtlMaterial * to, MtlMaterial * from) {
  if (!to) return NULL;
  if (!from) return NULL;
  (*to) = (*from);
  to->name    = objfile_strdup(from->name);
  to->map_Ka  = objfile_strdup(from->map_Ka);
  to->map_Kd  = objfile_strdup(from->map_Kd);
  to->map_Kn  = objfile_strdup(from->map_Kn);

  return to; 
}

#define OBJFACE_INIT_V_ONLY   0
#define OBJFACE_INIT_HAVE_VT  1
#define OBJFACE_INIT_HAVE_N   2
#define OBJFACE_INIT_HAVE_ALL 3




#define OBJFILE_ARRAY_GROW 1024

struct ObjFile_  {
  Vec3d   * vt;
  int       n_vt;
  int       s_vt;
  
  Vec3d   * v;
  int       n_v;
  int       s_v;
  
  ObjFace * f;
  int       n_f;
  int       s_f;
  
  /* material list */  
  MtlMaterial * mtl;
  int           n_mtl;  
  int           s_mtl;  
  
  /* Currently active material, object and group. */
  MtlMaterial * usemtl;
  ObjGroup    * group;
  ObjObject   * object;

};


ObjFace * objface_init_va(ObjFace * me, ObjFile * file, 
  int flags, int n_points, va_list args) {
  int index    = 0;
  me->n_points = n_points;
  me->points   = calloc(me->n_points, sizeof(ObjFacePoint));
  for( index = 0 ; index < n_points; index++) {
    ObjFacePoint * point = me->points + index;
    int i_v = -1, i_vt = -1, i_n = -1;
    i_v = va_arg(args, int);
    if (flags & OBJFACE_INIT_HAVE_VT) {
      i_vt = va_arg(args, int);
    }
    if (flags & OBJFACE_INIT_HAVE_N) {
      i_n = va_arg(args, int);
    }
    point->i_v  = i_v;
    point->i_vt = i_vt;    
    point->i_n  = i_n;
    point->grp  = file->group;
    point->obj  = file->object;
    point->mtl  = file->usemtl;
  }
  return me;
}

ObjFace * objface_init(ObjFace * me, ObjFile * file, int flags, int n_points, ...) 
{
  ObjFace * face = NULL;
  va_list args;
  va_start(args, n_points);
  objface_init_va(me, file, flags, n_points, args);
  va_end(args);
  return face;
}


ObjFace * objface_done(ObjFace * me) {
  if (!me) return NULL;
  free(me->points);
  me->points = NULL;
  return me;
}



ObjFile * objfile_alloc() {
  return calloc(sizeof(ObjFile), 1);
}



ObjFile * objfile_init(ObjFile * me) {
  if (!me) return NULL;
  
  me->vt    = NULL;
  me->n_vt  = 0;
  me->s_vt  = 0;
  
  me->v     = NULL;
  me->n_v   = 0;
  me->s_v   = 0;
  
  me->f     = NULL;
  me->n_f   = 0;
  me->s_f   = 0;
  
  me->mtl   = NULL;
  me->s_mtl = 0;
  me->n_mtl = 0;
  
  return me;
}

ObjFile * objfile_done(ObjFile * me) {
  int index;
  if (!me) return NULL;
  free(me->vt);
  free(me->v);
  
  for (index = 0; index < me->n_f; index ++) {
    objface_done(me->f + index);
  }
  
  for (index = 0; index < me->n_mtl; index ++) {
    mtlmaterial_done(me->mtl + index);
  }
  
  free(me->f);
  free(me->mtl);
  objfile_init(me);
  return me;
}


ObjFile * objfile_free(ObjFile * me) {
  objfile_done(me);
  free(me);
  return NULL;
}

ObjFile *  objfile_new() {
  return objfile_init(objfile_alloc());
}

#define OBJFILE_GROW_ARRAY(ARR, SIZE, SPACE, BY)                          \
  if (SIZE  >= SPACE) {                                                   \
    int new_s   = SPACE + BY;                                             \
    void * aid  = realloc(ARR, new_s * sizeof (*ARR));                    \
    if (!aid) return -1;                                                  \
    SPACE       = new_s;                                                  \
    ARR         = aid;                                                    \
  }                                                                       \
  return SPACE;


int objfile_grow_v(ObjFile * me) {
  OBJFILE_GROW_ARRAY(me->v, me->n_v, me->s_v, OBJFILE_ARRAY_GROW);
}

int objfile_grow_vt(ObjFile * me) {
  OBJFILE_GROW_ARRAY(me->vt, me->n_vt, me->s_vt, OBJFILE_ARRAY_GROW);
}

int objfile_grow_f(ObjFile * me) {
  OBJFILE_GROW_ARRAY(me->f, me->n_f, me->s_f, OBJFILE_ARRAY_GROW);
}

int objfile_grow_mtl(ObjFile * me) {
  OBJFILE_GROW_ARRAY(me->mtl, me->n_mtl, me->s_mtl, OBJFILE_ARRAY_GROW);
}

int mtlmaterial_compare(const void * m1, const void * m2) {
  const MtlMaterial * mat1 = m1;
  const MtlMaterial * mat2 = m2;
  return strcmp(mat1->name, mat2->name);  
}


int objfile_add_v(ObjFile * me , float x, float y, float z) {
  if (objfile_grow_v(me) < 0) return -1;
  me->v[me->n_v].x = x;
  me->v[me->n_v].y = y;  
  me->v[me->n_v].z = z;
  me->n_v++;
  return me->n_v - 1;  
}

int objfile_add_vt(ObjFile * me , float u, float v, float w) {
  if (objfile_grow_vt(me) < 0) return -1;
  me->vt[me->n_vt].x = u;
  me->vt[me->n_vt].y = v;  
  me->vt[me->n_vt].z = w;
  me->n_vt++;
  return me->n_vt - 1;
}


int objfile_add_f(ObjFile * me , ObjFace * face) {
  if (objfile_grow_f(me) < 0) return -1;
  me->f[me->n_f] = * face;
  me->n_f++;
  return me->n_f - 1;
}

int objfile_add_mtl(ObjFile * me , MtlMaterial * material) {
  if (objfile_grow_mtl(me) < 0) return -1;
  mtlmaterial_copy(me->mtl + me->n_mtl, material);
  me->n_mtl++;
  qsort(me->mtl, me->n_mtl, sizeof(*me->mtl), mtlmaterial_compare);  
  return me->n_mtl - 1;
}

MtlMaterial * objfile_get_mtl(ObjFile * me, char * name) {
  MtlMaterial probe = { 0 };
  probe.name = name;
  return bsearch(&probe, me->mtl, me->n_mtl, sizeof(*me->mtl), mtlmaterial_compare);  
}




ObjFile * objfile_load_filename(char * filename);

int objfile_get_vertex_count(ObjFile * me) {
  return me->n_v;
}

int objfile_get_uv_count(ObjFile * me) {
  return me->n_v;
}

int objfile_get_face_count(ObjFile * me) {
  return me->n_f;
}

int objfile_get_face_point_count(ObjFile * me, int index) {
  ObjFace * face;
  if (!me) return -2;
  if (index < 1) return -1;
  if (index > me->n_f) return -1;
  face = me->f + index - 1;
  return face->n_points;
}

ObjFace * objfile_get_face(ObjFile * me, int index) {
  if (!me) return NULL;
  if (index < 1) return NULL;
  if (index > me->n_f) return NULL;
  return me->f + index - 1;
}

ObjFacePoint * objfile_get_face_point(ObjFile * me, int index, int pindex) {
  ObjFace * face;
  if (pindex < 0) return NULL;
  face = objfile_get_face(me, index);
  if (!face) return NULL;;
  if (pindex >= face->n_points) return NULL;
  return face->points + pindex;
}

int objfile_get_face_point_iv(ObjFile * me, int index, int pindex) {
  ObjFacePoint * point = objfile_get_face_point(me, index, pindex);
  if (!point) return -1;
  return point->i_v;
}

int objfile_get_face_point_ivt(ObjFile * me, int index, int pindex) {
  ObjFacePoint * point = objfile_get_face_point(me, index, pindex);
  if (!point) return -1;
  return point->i_vt;
}

int objfile_get_face_point_in(ObjFile * me, int index, int pindex) {
  ObjFacePoint * point = objfile_get_face_point(me, index, pindex);
  if (!point) return -1;
  return point->i_n;
}

Vec3d * objfile_get_vertex(ObjFile * me, int index) {
  if (!me) return NULL;
  if (index < 1) return NULL;
  if (index > me->n_v) return NULL;
  return me->v + index - 1;
}


Vec3d * objfile_get_uv(ObjFile * me, int index) {
  if (!me) return NULL;
  if (index < 1) return NULL;
  if (index > me->n_v) return NULL;
  return me->v + index - 1;
}


#define OBJFILE_LINE_SIZE 1024


ObjFile * objfile_load_mtllib_line(ObjFile * me, char * line, MtlMaterial * mat)  {
  float f1 = 0.0, f2 = 0.0, f3 = 0.0;
  float u = 0.0, v = 0.0, w = 0.0;
  char fc = line[0];
  char command[64];
  char name[256] = { '\0' };
  int v1 = 0, v2 = 0, v3 = 0, v4=0;
  int t1 = 0, t2 = 0, t3 = 0, t4=0;
  int n1 = 0, n2 = 0, n3 = 0;
  int i = 0;
  
  /* ignore comments */
  if (fc == '#') return me; 
    
  /* newmtl statement  */
  if (sscanf(line, "newmtl %255s", name) == 1) {
    /* Store old material if we have it. */
    objfile_add_mtl(me, mat);
    /* Clean up and reinit material. */
    mtlmaterial_done(mat);
    mtlmaterial_init_empty(mat);
    /* and set name again. */
    mat->name = objfile_strdup(name);
    return me;
  }
    
  /* Ka statements  */
  if (sscanf(line, "Ka %f %f %f", &f1, &f2, &f3) == 3) {
    mat->Ka[0] = f1; mat->Ka[1] = f2; mat->Ka[2] = f3;
    return me;
  }
  
  /* Ks statements  */
  if (sscanf(line, "Ks %f %f %f", &f1, &f2, &f3) == 3) {
    mat->Ks[0] = f1; mat->Ks[1] = f2; mat->Ks[2] = f3;
    return me;
  }
  
  /* Kd statements  */
  if (sscanf(line, "Kd %f %f %f", &f1, &f2, &f3) == 3) {
    mat->Kd[0] = f1; mat->Kd[1] = f2; mat->Kd[2] = f3;
    return me;
  }
  
  /* illum statements  */
  if (sscanf(line, "illum %d", &i) == 1) {
    mat->illum = i;
    return me;
  }
  
  /* Ns statements  */
  if (sscanf(line, "Ns %f", &f1) == 1) {
    mat->Ns = f1;
    return me;
  }
  
  /* d statements  */
  if (sscanf(line, "d %f", &f1) == 1) {
    mat->Tr = f1;
    return me;
  }
  
  /* Tr statements  */
  if (sscanf(line, "Tr %f", &f1) == 1) {
    mat->Tr = f1;
    return me;
  }
  
  /* map_Ka statements  */
  if (sscanf(line, "map_Ka %255s", name) == 1) {
    if (mat->map_Ka) free(mat->map_Ka);
    mat->map_Ka = objfile_strdup(name);
    return me;
  }
  
  /* map_Kd statements  */
  if (sscanf(line, "map_Kd %255s", name) == 1) {
    if (mat->map_Kd) free(mat->map_Kd);
    mat->map_Kd = objfile_strdup(name);
    return me;
  }
  
  /* map_Kn statements  */
  if (sscanf(line, "map_Kn %255s", name) == 1) {
    if (mat->map_Kn) free(mat->map_Kn);
    mat->map_Kn = objfile_strdup(name);
    return me;
  }
  
  LOG_ERROR("Unknown instruction >%s< in mtl file", line); 
    
  return NULL;
}



ObjFile * objfile_parse_line(ObjFile * me, char * line) {
  float f1 = 0.0, f2 = 0.0, f3 = 0.0;
  float u = 0.0, v = 0.0, w = 0.0;
  char fc = line[0];
  char command[64];
  char name[256] = { '\0' };
  int v1 = 0, v2 = 0, v3 = 0, v4=0;
  int t1 = 0, t2 = 0, t3 = 0, t4=0;
  int n1 = 0, n2 = 0, n3 = 0;
  int i = 0;
  
  /* ignore comments */
  if (fc == '#') return me; 
  
  if (sscanf(line, "v %f %f %f", &f1, &f2, &f3) == 3) {
    if (objfile_add_v(me, f1, f2, f3) < 0) return NULL;
    return me;    
  } 
  
  if (sscanf(line, "vt %f %f %f", &u, &v, &w) >= 2) {
    if (objfile_add_vt(me, u, v, w) < 0) return NULL;
    return me;    
  }

  /* Simple quad polygon. */
  if (sscanf(line, "f %d %d %d %d", &v1, &v2, &v3, &v4) == 4) {
    ObjFace face;
    objface_init(&face, me, OBJFACE_INIT_V_ONLY, 4, v1, v2, v3, v4);
    if (objfile_add_f(me, &face) < 0) return NULL;
    return me;
  }

  /* Textured quad polygon. */
  if (sscanf(line, "f %d/%d %d/%d %d/%d %d/%d", 
            &v1, &t1, &v2, &t2, &v3, &t3, &v4, &t4) == 8) {       
    ObjFace face;
    objface_init(&face, me, OBJFACE_INIT_HAVE_VT, 4, v1, t1, v2, t2, v3, t3, v4, t4);
    if (objfile_add_f(me, &face) < 0) return NULL;
    return me;
  }

  
  /* Simple triangle polygon. */
  if (sscanf(line, "f %d %d %d", &v1, &v2, &v3) == 3) {
    ObjFace face;
    objface_init(&face, me, OBJFACE_INIT_V_ONLY, 3, v1, v2, v3);
    if (objfile_add_f(me, &face) < 0) return NULL;
    return me;
  }
    
  /* Textured triangle polygon. */
  if (sscanf(line, "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3) == 6) {       
    ObjFace face;
    objface_init(&face, me, OBJFACE_INIT_HAVE_VT, 3, v1, t1, v2, t2, v3, t3);
    if (objfile_add_f(me, &face) < 0) return NULL;
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
  if (sscanf(line, "mtllib %255s", name)  == 1) {
    return me;
  }
  
  /* usemtl statements  */
  if (sscanf(line, "usemtl %255s", name) == 1) {
    me->usemtl = objfile_get_mtl(me, name);
    return me;
  }

  LOG_ERROR("Unknown instruction >%s< in OBJ file", line); 
  return NULL;
}





ObjFile * objfile_parse_file(ObjFile * me, FILE * file) {
  /* It's reasonable to use a static buffer since line sizes shoun't get THAT long for an obj file. */
  char buffer[OBJFILE_LINE_SIZE];
  
  while (!feof(file)) {
    char * res = fgets(buffer, sizeof(buffer), file); 
    if (!res) {
      if (ferror(file)) {
        LOG_ERROR("Read error when reading in OBJ file.");
        return NULL;
      } else {
        return me;
      }
    }
    if (!objfile_parse_line(me, buffer)) { 
      return NULL;
    }
  }
    
  return me;
}



ObjFile * objfile_load_file(FILE * file) {
  ObjFile * me = objfile_new();
  if (!objfile_parse_file(me, file)) {
    return objfile_free(me);
  }
  return me;
}

ObjFile * objfile_load_filename(char * filename) {
  ObjFile * me;
  FILE * file = fopen(filename, "rt");
  if (!file) {
    LOG_ERROR("Cannot open obj file %s\n", filename);
    return NULL;
  }
  
  me = objfile_load_file(file);
  
  if (!me) {
    LOG_ERROR("Parse error or out of memory in obj file %s\n", filename);
  } else {
    LOG_NOTE("Loaded model from %s with %d points and %d tris and %d uvs\n", 
          filename, me->n_v, me->n_f, me->n_vt);
  }
  
  fclose(file);
  return me;
}


