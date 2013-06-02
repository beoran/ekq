// #include <libxml/parser.h>

#include "eruta.h"
#include "state.h"
#include "react.h"
#include "image.h"
#include "tile.h"
#include "tilepane.h"
#include "tilemap.h"
#include "tileio.h"
#include "sound.h"
#include "silut.h"
#include "fifi.h"
#include "ui.h"
#include "hatab.h"
/* #include "beo.h" */
#include "assert.h"
#include "str.h"
#include "sprite.h"
#include "alps.h"





#define SCREEN_W 640
#define SCREEN_H 480
ALLEGRO_FONT* font;
ALLEGRO_TRANSFORM identity;
ALLEGRO_BITMAP* buffer;
ALLEGRO_BITMAP* texture;
ALLEGRO_COLOR   solid_white;

#define abort_example perror
#define ERUTA_TEST_SHEET      "data/image/tile/tiles_village_1000.png"
#define ERUTA_MAINMENU_IMAGE  "data/image/background/eruta_mainmenu.png"
/** For some reason, onlt wav music plays?
It seems there is a bug in init_dynlib() in the ogg driver nbut only
if Allegro is compiled in the default RelWithDbg mode.
***/
#define ERUTA_TEST_MUSIC "data/music/musictest.ogg"
#define ERUTA_MAP_TEST "data/map/map_0001.tmx"
// #define RUN_TESTS 1

/*
#ifdef RUN_TESTS

#endif
*/



/* Testing function for al_get_standard_path */
void puts_standard_path(int path, char * name) {
  ALLEGRO_PATH * testpath;
  testpath =  al_get_standard_path(path);
  puts(name);
  puts(al_path_cstr(testpath, ALLEGRO_NATIVE_PATH_SEP));
  al_destroy_path(testpath);
}
 
 
React * main_react_key_down(React * self, ALLEGRO_KEYBOARD_EVENT * event) {  
  Point f = bevec0();
  State * state = (State *) self->data;
  Camera * camera = NULL;
  Thing * actor_data;
  if (!state) return NULL;
  camera = state_camera(state);
  if (!camera) return NULL;
  actor_data = state_actor(state);
  if (actor_data) {
    f = thing_v(actor_data);
  }
  switch(event->keycode) {
    case ALLEGRO_KEY_UP:
      f.y = -100;
    break;
    case ALLEGRO_KEY_DOWN:
      f.y = +100;
    break;
    case ALLEGRO_KEY_LEFT:
      f.x = -100;
    break;
    case ALLEGRO_KEY_RIGHT:
      f.x = +100;
    break;
    case ALLEGRO_KEY_F1:
      bbconsole_active_(state_console(state), TRUE);
    break;
    case ALLEGRO_KEY_F2:
      bbconsole_active_(state_console(state), FALSE);
    break;  
    case ALLEGRO_KEY_ESCAPE:
    case ALLEGRO_KEY_ENTER:
      state_done(state);
    default:
    break;
  }
  if(actor_data) {
    thing_v_(actor_data, f);
  } 
  return self;
}


React * main_react_key_up(React * self, ALLEGRO_KEYBOARD_EVENT * event) {
  Point f         = bevec0();
  State * state   = (State *) self->data;
  Camera * camera = NULL;
  Thing  * actor_data = NULL;
  if (!state) return NULL;
  camera = state_camera(state);
  if (!camera) return NULL;
  // os = camera_speed(camera);
  actor_data = state_actor(state);
  if (actor_data) {
    f = thing_v(actor_data);
  }
  switch(event->keycode) {
    case ALLEGRO_KEY_UP:
      f.y = 0.0;
    break;
    case ALLEGRO_KEY_DOWN:
      f.y = 0.0;
    break;
    case ALLEGRO_KEY_LEFT:
      f.x = 0.0;
    break;
    case ALLEGRO_KEY_RIGHT:
      f.x = 0.0;
    break;
    default:
    break;
  }
  if (actor_data) {
    thing_v_(actor_data, f);
  } else {
    puts("No actor data!");
  }
  return self;
}



int real_main(void) {
    Image    * border   = NULL;
    Image    * sheet    = NULL;
    Tileset  * tileset  = NULL;
    Tile     * tile     = NULL;
    State    * state    = NULL;
    Music    * music    = NULL;
    Camera   * camera   = NULL;
    Tilepane * tilepane = NULL;
    Tilemap  * map      = NULL;
    Thing    * actor    = NULL;
    Tracker  * tracker          = NULL;
    Tracker  * maptracker       = NULL;
    Sprite   * sprite           = NULL;
    SpriteState * spritestate   = NULL;
    AlpsShower shower;
    int        actor_id         = -1;
    int        sprite_id        = -1;
    
    React    react;
    ALLEGRO_COLOR myblack = {0.0, 0.0, 0.0, 1.0};
    
    
    state = state_alloc();
    state_set(state); 
    // set the state as the global state.
    /*Point      mp = { -100, -100};
    Point      mv = {0    , 0};*/
    if((!(state)) || (!state_init(state, FALSE))) {
      perror(state_errmsg(state));
      return 1;
    }
    
    alpsshower_init(&shower, state_camera(state), 100, 1.0, bevec(10.0, 10000.0));
 
 
    /* Initializes the reactor, the game state is it's data. */
    react_initempty(&react, state);
    react.keyboard_key_up   = main_react_key_up;
    react.keyboard_key_down = main_react_key_down;
    
    puts_standard_path(ALLEGRO_EXENAME_PATH, "ALLEGRO_EXENAME_PATH:");

    camera  = state_camera(state);
    //music   = music_load("musictest.ogg");
    // if(!music) perror("musictest.ogg");
    /* Initialize empty sprite and load a few layers. */
    sprite_id   = 1;
    sprite      = state_getornewsprite(state, sprite_id);
    spritestate = spritestate_new(sprite);
    if(state_sprite_loadulpcss(state, 1, 0, "image/ulpcss/body/male/light.png") < 0 
      ) { 
        fprintf(stderr, "Could not load body layer.\n");
    }
    state_sprite_loadulpcss(state, 1, 1, "image/ulpcss/hair/male/messy2/redhead.png");
    state_sprite_loadulpcss(state, 1, 2, "image/ulpcss/torso/white_shirt_male.png");
    state_sprite_loadulpcss(state, 1, 3, "image/ulpcss/legs/green_pants_male.png");
    state_sprite_loadulpcss(state, 1, 4, "image/ulpcss/feet/brown_shoes_male.png");
    /*
    spritestate_now_(spritestate, 0, 0);
    if(spritestate_posedirection_(spritestate, SPRITE_WALK, SPRITE_EAST)) {
      fprintf(stderr, "Could not set sprite pose!\n");
    } else {
      printf("Sprite pose set.\n");
    }
    */

    /*border  = fifi_loadbitmap("border_004.png",
                            "image", "ui", "background", NULL);*/
    state_loadtilemap_vpath(state, "map/map_0001.tmx");
    map = state_nowmap(state);
    if(!map) {
      puts("Map is NULL!");
    } else {
      actor_id = state_newthingindex(state, THING_ACTOR, 120, 100, 1, 32, 32);
      if(actor_id >= 0) {
        state_camera_track_(state, actor_id);
      }
      state_lockin_maplayer(state, 0);
    }

  // Try to load the mainruby file.
  rh_runfilename_console(state_console(state), "main.rb", state_ruby(state));
  // rh_runfilename_stderr("main.rb", state_ruby(state));

  // Call the on_start function.
  { 
    USTR * com = ustr_newf("on_start('%s')", "OK!");
    rh_dostring_console(state_console(state), (char *) ustr_c(com), state_ruby(state));
    // rh_dostring_stderr(ustr_c(com), state_ruby(state));
    ustr_free(com);
  }
  
  if ((actor_id >= 0) && (sprite_id >= 0)) {
    state_actorindex_(state, actor_id);
    state_thing_sprite_(state, actor_id, sprite_id);
    state_thing_pose_(state, actor_id, SPRITE_WALK);
  }
  // spritestate_speedup_(spritestate, 2.0);

  /* Main game loop, controlled by the State object. */  
  while(state_busy(state)) { 
      mrb_value mval;
      Point spritenow = bevec(100, 120); 
      react_poll(&react, state);
      alpsshower_update(&shower, state_frametime(state));
      state_update(state);
      state_draw(state);
      state_flip_display(state);
   }
   /* image_free(border); */
   state_done(state);
   state_free(state); 
   return 0;
}


int main(int argc, char* argv[]) {
  int res; // init xml parser
  // LIBXML_TEST_VERSION
  res = real_main();
  // cleanup xml parser
  // xmlCleanupParser();
  return res;
}
