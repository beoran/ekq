/*
* toruby.c helps expose functionality from C to Mruby for Eruta.
* All functions are prefixed with tr_.
* Look at the tr_*.c files.
* */

#include "eruta.h"
#include "toruby.h"
#include "rh.h"
#include "state.h"
#include "image.h"
#include "fifi.h"
#include "store.h"
#include "scegra.h"
#include "sound.h"
#include "camera.h"
#include "monolog.h"
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include "tr_macro.h"
#include "tr_audio.h"
#include "tr_graph.h"
#include "tr_store.h"
#include "tr_sprite.h"


/* Documentation of mrb_get_args: 
 
  retrieve arguments from mrb_state.

  mrb_get_args(mrb, format, ...)

  returns number of arguments parsed.

  format specifiers:

   o: Object [mrb_value]
   S: String [mrb_value]
   A: Array [mrb_value]
   H: Hash [mrb_value]
   s: String [char*,int]
   z: String [char*] nul terminated
   a: Array [mrb_value*,mrb_int]
   f: Float [mrb_float]
   i: Integer [mrb_int]
   b: Binary [int]
   n: Symbol [mrb_sym]
   &: Block [mrb_value]
   *: rest argument [mrb_value*,int]
   |: optional
 */



/**
* helper macros 
*/


void tr_Font_free(mrb_state * mrb, void * ptr) {
  (void) mrb;

  LOG("Freeing font %p\n", ptr);
  font_free((Font*)ptr);
}

struct mrb_data_type toruby_Font_type = { "Font", tr_Font_free };

/*
mrb_value toruby_Font(mrb_state* mrb, mrb_value sel, mrb_value rname) {
  Fonr * font = RSTRING(name);
}
*/

void tr_Path_free(mrb_state * mrb, void * ptr) {
  (void) mrb;
  LOG("Freeing path %p\n", ptr);
  al_destroy_path((Path*)ptr);
}

struct mrb_data_type toruby_Path = { "Path", tr_Path_free };

/* gets a data path based on a vpath. */
mrb_value tr_Path(Ruby * ruby, mrb_value self, struct RClass * klass) {
  ALLEGRO_PATH * path = NULL;
  const char * vpath = NULL; int vlen = 0;
  (void) self;
   
  mrb_get_args(ruby, "s", &vpath, &vlen);
  LOG("Making path for : %s, %d", vpath, vlen);
  path = fifi_data_vpath(vpath);
  if(!path) return mrb_nil_value();
  return mrb_obj_value(Data_Wrap_Struct(ruby, klass, &toruby_Path, path));
}

/* Test method. */
static mrb_value tr_test(mrb_state * mrb, mrb_value self) {
  (void) self; (void) mrb;
  
  LOG("Hello from a mruby test!\n");
  return self;
}


/* Stops the engine by calling state_done */
static mrb_value tr_state_done(mrb_state * mrb, mrb_value self) {
  State   * state   = NULL;
  
  (void) self; (void) mrb;
  
  state             = state_get();
  if (state) {
    state_done(state);
    return self;
  } else {
    return mrb_nil_value();
  }
}

/** Writes a NOTE message to to log. */
static mrb_value tr_log(mrb_state * mrb, mrb_value self) {
  State   * state   = NULL;  
  (void) self; (void) mrb;
  
  mrb_value text    = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  
  LOG_NOTE("%s\n", RSTRING_PTR(text));
  return self;
}

/** Writes a messageto a certain log level log. */
static mrb_value tr_log_to(mrb_state * mrb, mrb_value self) {
  State   * state   = NULL;  
  (void) self; (void) mrb;
  
  mrb_value level   = mrb_nil_value();
  mrb_value text    = mrb_nil_value();

  mrb_get_args(mrb, "SS", &level, &text);
  
  LOG_LEVEL(RSTRING_PTR(level), "%s\n", RSTRING_PTR(text));
  return self;
}


/** Cause a warning to be logged */
static mrb_value tr_warn(mrb_state * mrb, mrb_value self) {
  State   * state   = NULL;  
  (void) self; (void) mrb;
  
  mrb_value text    = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  LOG_WARNING("%s\n", RSTRING_PTR(text));
  return self;
}


/** Enables a certain log level */
static mrb_value tr_log_enable(mrb_state * mrb, mrb_value self) {
  State   * state   = NULL;
  BBConsole * console = NULL;
  
  (void) self; (void) mrb;
  
  state             = state_get();
  mrb_value text    = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  monolog_enable_level(RSTRING_PTR(text));
  return self;
}

/** Disables a certain log level */
static mrb_value tr_log_disable(mrb_state * mrb, mrb_value self) {
  State   * state   = NULL;
  BBConsole * console = NULL;
  
  (void) self; (void) mrb;

  state             = state_get();  
  mrb_value text    = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  monolog_disable_level(RSTRING_PTR(text));
  return self;
}


/* Loads another script from the script directory. Reports to the 
 console or stderr if no console available. */
static mrb_value tr_script(mrb_state * mrb, mrb_value self) {
  int res; 
  char * command;
  State * state         = state_get();
  
  (void) self;

  
  mrb_value text        = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  command               = mrb_str_to_cstr(mrb, text);
  
  res = rh_run_script(mrb, command);
  return mrb_fixnum_value(res);
}


/* Gets the active maze's ID for the state   
static mrb_value tr_active_maze(mrb_state * mrb, mrb_value self) {
  int id;
  (void) self; (void) mrb;
  State * state   = state_get();  
  return state->active_maze_id;  
} 


static mrb_value tr_eventvalues(mrb_state * mrb   , ALLEGRO_EVENT * event, 
                          mrb_value * values, int size) {
  int result;
  mrb_value aid;
  aid = mrb_hash_new(mrb);
  // mrb_hash_set(mrb, aid, mrb_intern(mrb, "type"), );
  return aid;
}
*/


static mrb_value tr_camera_track(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int   thing_index;
  (void) self; (void) mrb;
  mrb_get_args(mrb, "i", &thing_index);  
  result = state_camera_track_(state, thing_index);
  return mrb_fixnum_value(result);
}

static mrb_value tr_lockin_maplayer(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_int   layer;
  (void) self; (void) mrb;
  mrb_get_args(mrb, "i", &layer);  
  result = state_lockin_maplayer(state, layer);
  return mrb_fixnum_value(result);
}

#define TORUBY_CAMERA_TYPE_GETTER(NAME, TOCALL, TYPE, CONVERT)  \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {  \
  State * state    = state_get();                         \
  Camera * camera  = state_camera(state);                 \
  TYPE result;                                            \
  (void) self; (void) mrb;                                \
  result           = TOCALL(camera);                      \
  return CONVERT(result);                                 \
}

#define TORUBY_CAMERA_TYPE_MRB_GETTER(NAME, TOCALL, TYPE, CONVERT)  \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {  \
  State * state    = state_get();                         \
  Camera * camera  = state_camera(state);                 \
  TYPE result;                                            \
  (void) self; (void) mrb;                                \
  result           = TOCALL(camera);                      \
  return CONVERT(mrb, result);                            \
}

#define TORUBY_CAMERA_TYPE_SETTER(NAME, TOCALL, TYPE, CONVERT, PARAMSTR)  \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {  \
  State * state    = state_get();                         \
  Camera * camera  = state_camera(state);                 \
  TYPE result, value;                                     \
  (void) self; (void) mrb;                                \
  mrb_get_args(mrb, PARAMSTR, &value);                    \
  result           = TOCALL(camera, value);               \
  return CONVERT(result);                                 \
}

#define TORUBY_CAMERA_TYPE_MRB_SETTER(NAME, TOCALL, TYPE, CONVERT, PARAMSTR)  \
static mrb_value NAME(mrb_state * mrb, mrb_value self) {  \
  State * state    = state_get();                         \
  Camera * camera  = state_camera(state);                 \
  TYPE result, value;                                     \
  (void) self; (void) mrb;                                \
  mrb_get_args(mrb, PARAMSTR, &value);                    \
  result           = TOCALL(camera, value);               \
  return CONVERT(mrb, result);                            \
}



#define TORUBY_CAMERA_FLOAT_GETTER(NAME, TOCALL)          \
  TORUBY_CAMERA_TYPE_MRB_GETTER(NAME, TOCALL, mrb_float, mrb_float_value)

#define TORUBY_CAMERA_INT_GETTER(NAME, TOCALL)          \
  TORUBY_CAMERA_TYPE_GETTER(NAME, TOCALL, mrb_int, mrb_fixnum_value)

#define TORUBY_CAMERA_FLOAT_SETTER(NAME, TOCALL)          \
  TORUBY_CAMERA_TYPE_MRB_SETTER(NAME, TOCALL, mrb_float, mrb_float_value, "f")

#define TORUBY_CAMERA_INT_SETTER(NAME, TOCALL)          \
  TORUBY_CAMERA_TYPE_SETTER(NAME, TOCALL, mrb_int, mrb_fixnum_value, "i")


  
TORUBY_CAMERA_FLOAT_GETTER(tr_camera_x, camera_at_x)
TORUBY_CAMERA_FLOAT_GETTER(tr_camera_y, camera_at_y)
TORUBY_CAMERA_FLOAT_GETTER(tr_camera_z, camera_at_z)
TORUBY_CAMERA_FLOAT_GETTER(tr_camera_alpha, camera_alpha)
TORUBY_CAMERA_FLOAT_GETTER(tr_camera_theta, camera_theta)


TORUBY_CAMERA_INT_GETTER(tr_camera_w, camera_w)
TORUBY_CAMERA_INT_GETTER(tr_camera_h, camera_h)

TORUBY_CAMERA_FLOAT_SETTER(tr_camera_x_, camera_at_x_)
TORUBY_CAMERA_FLOAT_SETTER(tr_camera_y_, camera_at_y_)
TORUBY_CAMERA_FLOAT_SETTER(tr_camera_z_, camera_at_z_)
TORUBY_CAMERA_FLOAT_SETTER(tr_camera_alpha_, camera_alpha_)
TORUBY_CAMERA_FLOAT_SETTER(tr_camera_theta_, camera_theta_)


/* Obsolete, tile maps will be loaded through store. 
static mrb_value tr_loadtilemap_vpath(mrb_state * mrb, mrb_value self) {
  State * state    = state_get();
  int result;
  mrb_value rvpath;
  mrb_get_args(mrb, "S", &rvpath);  
  result = state_loadtilemap_vpath(state, mrb_str_to_cstr(mrb, rvpath));
  return mrb_fixnum_value(result);
}
*/





/*
int state_newthingindex(State * state, int kind, 
                        int x, int y, int z, int w, int h);
int state_thing_sprite_(State * state, int thing_index, int sprite_index); 
int state_thing_pose_(State * state, int thing_index, int pose);
int state_thing_direction_(State * state, int thing_index, int direction);

int state_actorindex_(State * self, int thing_index);
Thing * state_actor(State * self);
*/

/*
tr_getornewsprite
tr_newsprite
tr_sprite
tr_sprite_loadulpcss
tr_newthing
tr_camera_track
tr_lockin_maplayer 
tr_loadtilemap_vpath
tr_thing_sprite_
tr_thing_pose_
tr_thing_direction_
tr_actorindex_
tr_actorindex
*/



TORUBY_0_FGETTER(tr_get_time, al_get_time)
TR_WRAP_NOARG_BOOL(tr_show_fps, global_state_show_fps)
TR_WRAP_NOARG_BOOL(tr_show_graph, global_state_show_graph)
TR_WRAP_NOARG_BOOL(tr_show_area, global_state_show_area)
TR_WRAP_B_BOOL(tr_show_fps_, global_state_show_fps_)
TR_WRAP_B_BOOL(tr_show_graph_, global_state_show_graph_)
TR_WRAP_B_BOOL(tr_show_area_, global_state_show_area_)
TR_WRAP_B_BOOL(tr_show_mouse_cursor_, scegra_show_system_mouse_cursor)



/* Initializes the functionality that Eruta exposes to Ruby. */
int tr_init(mrb_state * mrb) {
  // luaL_dostring(lua, "print 'Hello!' ");
  struct RClass *krn;
  struct RClass *pth;
  struct RClass *sto;
  struct RClass *gra;
  struct RClass *spr;
  struct RClass *thi;
  struct RClass *eru;
  struct RClass *aud;
  struct RClass *cam;
  
  eru = mrb_define_module(mrb, "Eruta");
  pth = mrb_define_class_under(mrb, eru, "Path"  , mrb->object_class);
  MRB_SET_INSTANCE_TT(pth, MRB_TT_DATA);
  
  
  /* Define some constants. */
  TR_CONST_INT(mrb, eru, "FLIP_HORIZONTAL", ALLEGRO_FLIP_HORIZONTAL); 
  TR_CONST_INT(mrb, eru, "FLIP_VERTICAL"  , ALLEGRO_FLIP_VERTICAL); 
  TR_CONST_INT(mrb, eru, "ALIGN_LEFT"     , ALLEGRO_ALIGN_LEFT);
  TR_CONST_INT(mrb, eru, "ALIGN_CENTRE"   , ALLEGRO_ALIGN_CENTRE);
  TR_CONST_INT(mrb, eru, "ALIGN_CENTER"   , ALLEGRO_ALIGN_CENTER);
  TR_CONST_INT(mrb, eru, "ALIGN_RIGHT"    , ALLEGRO_ALIGN_RIGHT);
  TR_CONST_INT(mrb, eru, "ALIGN_INTEGER"  , ALLEGRO_ALIGN_INTEGER);
  TR_CLASS_METHOD_NOARG(mrb, eru, "quit"  , tr_state_done);
  
  
  
  krn = mrb_module_get(mrb, "Kernel");
  if(!krn) return -1;
  TR_METHOD_ARGC(mrb, krn, "test"         , tr_test   , 1);
  TR_METHOD_ARGC(mrb, krn, "warn"         , tr_warn   , 1);
  TR_METHOD_ARGC(mrb, krn, "warning"      , tr_warn   , 1);
  TR_METHOD_ARGC(mrb, krn, "log"          , tr_log    , 1);
  TR_METHOD_ARGC(mrb, krn, "log_to"       , tr_log_to , 2);

  TR_METHOD_ARGC(mrb, krn, "log_enable"   , tr_log_disable , 1);
  TR_METHOD_ARGC(mrb, krn, "log_disable"  , tr_log_enable  , 1);
  TR_METHOD_ARGC(mrb, krn, "script"       , tr_script , 1);

  cam = mrb_define_module_under(mrb, eru, "Camera");

  TR_METHOD_ARGC(mrb, krn, "camera_track" , tr_camera_track, 1);
  TR_METHOD_ARGC(mrb, krn, "camera_lockin", tr_lockin_maplayer, 1);
  TR_METHOD_NOARG(mrb, krn, "camera_x"    , tr_camera_x);
  TR_METHOD_NOARG(mrb, krn, "camera_y"    , tr_camera_y);
  TR_METHOD_NOARG(mrb, krn, "camera_w"    , tr_camera_w);
  TR_METHOD_NOARG(mrb, krn, "camera_h"    , tr_camera_h);
  TR_METHOD_NOARG(mrb, krn, "camera_alpha", tr_camera_alpha);
  TR_METHOD_NOARG(mrb, krn, "camera_theta", tr_camera_theta);

  
 
  TR_CLASS_METHOD_NOARG(mrb, cam, "x"    , tr_camera_x);
  TR_CLASS_METHOD_NOARG(mrb, cam, "y"    , tr_camera_y);
  TR_CLASS_METHOD_NOARG(mrb, cam, "z"    , tr_camera_z);
  TR_CLASS_METHOD_NOARG(mrb, cam, "w"    , tr_camera_w);
  TR_CLASS_METHOD_NOARG(mrb, cam, "h"    , tr_camera_h);
  TR_CLASS_METHOD_NOARG(mrb, cam, "alpha", tr_camera_alpha);
  TR_CLASS_METHOD_NOARG(mrb, cam, "theta", tr_camera_theta);

  TR_CLASS_METHOD_ARGC(mrb, cam, "x="    , tr_camera_x_, 1);
  TR_CLASS_METHOD_ARGC(mrb, cam, "y="    , tr_camera_y_, 1);
  TR_CLASS_METHOD_ARGC(mrb, cam, "z="    , tr_camera_z_, 1);
  TR_CLASS_METHOD_ARGC(mrb, cam, "alpha=", tr_camera_alpha_, 1);
  TR_CLASS_METHOD_ARGC(mrb, cam, "theta=", tr_camera_theta_, 1);


  /*
  */ 
   
  
  TR_CLASS_METHOD_NOARG(mrb, eru, "show_fps", tr_show_fps);
  TR_CLASS_METHOD_NOARG(mrb, eru, "show_area", tr_show_area);
  TR_CLASS_METHOD_NOARG(mrb, eru, "show_graph", tr_show_graph);
  TR_CLASS_METHOD_ARGC(mrb, eru, "show_fps="  , tr_show_fps_, 1);
  TR_CLASS_METHOD_ARGC(mrb, eru, "show_area=" , tr_show_area_, 1);
  TR_CLASS_METHOD_ARGC(mrb, eru, "show_graph=", tr_show_graph_, 1);
  TR_CLASS_METHOD_ARGC(mrb, eru, "show_mouse_cursor=", tr_show_mouse_cursor_, 1);
  
  
  TR_CLASS_METHOD_NOARG(mrb, eru, "time", tr_get_time);
  

  
  /* Set up submodules. */
  tr_sprite_init(mrb, eru);
  tr_store_init(mrb, eru);
  tr_graph_init(mrb, eru);
  tr_audio_init(mrb, eru);

   
  // must restore gc area here ????
  mrb_gc_arena_restore(mrb, 0);
  
  return 0;
}








