#ifndef objfile_H_INCLUDED
#define objfile_H_INCLUDED

#include "vec3d.h"

/** A material of an OBJ/MTL file. */
struct MtlMaterial_ {
  char * name;
  char * map_Ka;
  char * map_Kd;
  char * map_Kn;
  
  float Ka[3];
  float Kd[3];
  float Ks[3];
  float Ns;
  float Tr;
  int   illum;
};

typedef struct MtlMaterial_ MtlMaterial;

/** A named object in an OBJ file */
struct ObjObject_ {
  char * name;
};

typedef struct ObjObject_ ObjObject;


/** A group  in an OBJ file */
struct ObjGroup_ {
  char * name;
};

typedef struct ObjGroup_ ObjGroup;


/** A point of an f face of an OBJ model */
struct ObjFacePoint_ {
  int i_v;
  int i_vt;
  int i_n;
  MtlMaterial * mtl; /* Material to use for this point, if any. */
  ObjObject   * obj; /* Object this point belongs to if any. */
  ObjGroup    * grp; /* Group this object belongs to if any. */
};

typedef struct ObjFacePoint_ ObjFacePoint;


/** an f face of a model */
struct ObjFace_ {
  int            n_points;
  ObjFacePoint   * points; 
};

typedef struct ObjFace_ ObjFace;



/* OBJ file loading. 
 * This doesn't produce a renderable model but load the data 
 * of the OBJ file as closely as is possible, to allow later 
 * conversion to the internal model format.
 */

typedef struct ObjFile_ ObjFile;


int objfile_add_mtl(ObjFile * me , MtlMaterial * material);
MtlMaterial * objfile_get_mtl(ObjFile * me, char * name);

ObjObject * objfile_get_object(ObjFile * me, char * name);
ObjGroup  * objfile_get_group(ObjFile * me, char * name);


ObjFace      * objfile_get_face(ObjFile * me, int index);
ObjFacePoint * objfile_get_face_point(ObjFile * me, int index, int pindex);

ObjFile *  objfile_new();

ObjFile * objfile_free(ObjFile * me);
ObjFile * objfile_load_filename(char * filename);
ObjFile * objfile_load_file(FILE * file);

ObjFile * objfile_load_filename(char * filename);


int objfile_get_vertex_count(ObjFile * me);
int objfile_get_uv_count(ObjFile * me);
int objfile_get_face_count(ObjFile * me);

int objfile_get_face_points(ObjFile * me, int index, int ** points);
Vec3d * objfile_get_vertex(ObjFile * me, int index);
Vec3d * objfile_get_uv(ObjFile * me, int index);






#endif
