/**
* This is a test for model in $package$
*/
#include "si_test.h"
#include "monolog.h"
#include "model.h"


TEST_FUNC(model) {
  int index;
  Model * me;  
  me = model_new();
  TEST_NOTNULL(me);
  for (index = 0 ; index < 6000 ; index ++) {
    model_add_vertex(me, index, index, index);
  }
  
  for (index = 0 ; index < 6000 ; index ++) {
    model_add_uv(me, index / 1000.0, index / 1000.0);
  }
  
  for (index = 0 ; index < 6000 ; index +=3) {
    model_add_triangle(me, index, index + 1, index+2);
  }
  
  for (index = 0 ; index < 6000 ; index ++) {
    model_set_rgba(me, index, index % 255, index % 255, index % 255, index % 265);
  }
  
  for (index = 0 ; index < 6000 ; index ++) {
    model_set_uv(me, index, index / 1000.0, index / 1000.0);
  }
  
  model_free(me);
  TEST_DONE();
}



TEST_FUNC(model_load) {
  int index;
  Model * me;  
  me = model_load_obj_filename("data/model/walltorch/torch.obj");
  TEST_NOTNULL(me);
  model_free(me);
  TEST_DONE();
}


DEFINE_STDERR_LOGGER(test_stderr_logger);
DEFINE_FILE_LOGGER(test_file_logger);


int main(void) {
  TEST_INIT();
  monolog_init();
  // initialize logging first
  // Initialize loggers
  monolog_add_logger(NULL, &test_stderr_logger);
  monolog_add_logger(fopen(__FILE__ ".log", "a"), &test_file_logger);

  TEST_RUN(model);
  TEST_RUN(model_load);
  TEST_REPORT();
  monolog_done();
}



