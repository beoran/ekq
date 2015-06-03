/**
* This is a test for objfile in $package$
*/
#include "si_test.h"
#include "objfile.h"


TEST_FUNC(objfile) {
  TEST_DONE();
}

TEST_FUNC(objfile_mtl) {
  MtlMaterial mat = { 0 }, *pmat;
  ObjFile * me = objfile_new();
  TEST_NOTNULL(me);
  mat.name    = "hello";
  mat.map_Kd  = "world";
  TEST_INTEQ(objfile_add_mtl(me, &mat), 0);

  mat.name    = "another";
  mat.map_Kd  = "green_world";
  TEST_INTEQ(objfile_add_mtl(me, &mat), 1);

  pmat        = objfile_get_mtl(me, "hello");
  TEST_NOTNULL(pmat);
  if (pmat) {
    TEST_STREQ("hello", pmat->name);
    TEST_STREQ("world", pmat->map_Kd);
  }

  pmat        = objfile_get_mtl(me, "another");
  TEST_NOTNULL(pmat);
  if (pmat) {
    TEST_STREQ("another", pmat->name);
    TEST_STREQ("green_world", pmat->map_Kd);
  }
  
  pmat        = objfile_get_mtl(me, "xyz");
  TEST_NULL(pmat);

  
  objfile_free(me);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(objfile);
  TEST_RUN(objfile_mtl);
  TEST_REPORT();
}



