/**
* This is a test for model in $package$
*/
#include "si_test.h"
#include "monolog.h"
#include "model.h"
#include <allegro5/allegro.h>


TEST_FUNC(model) {
  int index;
  Model * me;  
  me = model_new();
  TEST_NOTNULL(me);
  for (index = 0 ; index < 6000 ; index ++) {
    model_add_vertex(me, index, index, index);
  }
    
  for (index = 0 ; index < 6000 ; index +=3) {
    model_add_triangle(me, index, index + 1, index+2);
  }
  
  for (index = 0 ; index < 6000 ; index ++) {
    model_set_rgba(me, index, index % 255, index % 255, index % 255, index % 265);
  }
  
  model_free(me);
  TEST_DONE();
}



TEST_FUNC(model_load) {
  int index;
  Model * me;  
  me = model_load_obj_filename("data/model/cube/cube.obj");
  TEST_NOTNULL(me);
  model_free(me);
  TEST_DONE();
}


DEFINE_STDERR_LOGGER(test_stderr_logger);
DEFINE_FILE_LOGGER(test_file_logger);


int main(void) {
  ALLEGRO_DISPLAY * display;
  TEST_INIT();
  monolog_init();
  // initialize logging first
  // Initialize loggers
  monolog_add_logger(NULL, &test_stderr_logger);
  monolog_add_logger(fopen(__FILE__ ".log", "a"), &test_file_logger);
  al_init();
  al_init_primitives_addon();
  display = al_create_display(64, 64);
  // Must create a display to use vertex declarations, annoyingly so.

  TEST_RUN(model);
  TEST_RUN(model_load);
  TEST_REPORT();
  al_destroy_display(display);
  monolog_done();
}



