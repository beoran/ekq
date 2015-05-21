#include "eruta.h"
#include "mem.h"
#include "state.h"
#include "camera.h"
#include "sound.h"
#include "dynar.h"
#include "draw.h"
#include "fifi.h"
#include "rh.h"
#include "toruby.h"
#include "event.h"
#include "widget.h"
#include "sprite.h"
#include "scegra.h"
#include "monolog.h"
#include "callrb.h"
#include "store.h"

/* The data struct contains all global state and other data of the application.
*/
struct State_ {
  /* State flags. */
  BOOL                  busy;
  BOOL                  fullscreen;
  BOOL                  audio;
  
  /* Graphics mode. XXX: I think???? :P */
  int32_t               modeno;
  
  /* 
   There are two fonts for now, ttf and font. Font is a plain font
   for emergency use, ttf is the normal font for the console.
  */
  ALLEGRO_FONT        * font;
  ALLEGRO_FONT        * ttf;
  /* Some default colors */
  ALLEGRO_COLOR         colors[STATE_COLORS];
  
  
  
  /* Display */
  ALLEGRO_DISPLAY     * display;
  
  ALLEGRO_EVENT_QUEUE * queue;
  char                * errmsg;
  
  
  /* FPS handling. */
  double                fpsnow, fpstime, fps;
  int                   frames;
  
  /* Background image that can be set behind the tile map. */
  ALLEGRO_BITMAP      * background_image;
  
  /* Background color, in case of no tile map */
  ALLEGRO_COLOR         background_color;
  
  
  /* Active maze, linked from and loaded through a Store ID. */
  int                   active_maze_id;
  
  /* Does the area needs to be displayed or not. */
  int                   show_area;
  
  /* Does the graph needs to be displayed or not */
  int                   show_graph;
  
  /* Does the FPS counter needs to be displayed or not? */
  int                   show_fps;
  
  // View camera for the area, tile map and particle engine. 
  Camera              * camera;
  
  /* Mode is removed, this will be handled on the scripting side. */
    
  /* Sprite subsystem */
  SpriteList          * sprites;
  /* Ruby subsystem  */
  Ruby                * ruby;  
  /* 
   The ruby and error message GUI console.
   Implemented in C so it's usable even if there are script bugs.   
  */
  BBConsole           * console;
  
};






/** State variable.   */
static State * global_state_ = NULL;

/** Various loggers. One for stdout if not on windows,
 * one to a file and one to the BBConsole */
int state_console_logf(char * file, int line, char * level,
                       void * data, char * format, va_list args) {
  BBConsole * console = data;
  if (console) {
  bbconsole_printf(console, "%s %s %d:", level, file, line);
   return bbconsole_vprintf(console, format, args);
  }
  return -1;
}



DEFINE_STDERR_LOGGER(state_stderr_logger);
DEFINE_FILE_LOGGER(state_file_logger);
DEFINE_LOGGER(state_console_logger, state_console_logf, NULL);
 

/** Gets the global state data, or NULL if not set. */
State * state_get() {
  return global_state_;
}

/** Sets the given state data as the global state data for this apllication.
* returns the *previous* state data pointer or NULL if it was not set yet.
*/
State * state_set(State * state) {
  State * oldstate    = global_state_;
  global_state_ = state;
  return oldstate;
}


/** Return's the state's sprite list */
SpriteList * state_sprites(State * state) {
  if (!state) return NULL;
  return state->sprites;
}


/** Allocates a state struct */
State * state_alloc() {
  return STRUCT_ALLOC(State);
}

/** Frees a state struct. */
void state_free(State * self) {
  
  /* Shut down audio */
  audio_done();
  
  spritelist_free(self->sprites);
  self->sprites = NULL;
  rh_free(self->ruby);
  bbconsole_free((BBWidget *)self->console, NULL);
  self->console = NULL; /* disable console immediately. */
  /* Deallocate stored objects. */
  store_done();
 
  // font_free(self->font);
  al_destroy_display(self->display);
  camera_free(self->camera);

  al_uninstall_system();
  
  /* End logging. */
  monolog_done();  
  STRUCT_FREE(self);
}

/** Sets error message for state and returns NULL. */
State * state_errmsg_(State * state, char * mesg) {
  state->errmsg = mesg;
  return NULL;
}

/** Gets error message for state. */
char * state_errmsg(State * state) {
  return state->errmsg;
}


/** Registers an event source for this state */
State * state_eventsource(State * state, ALLEGRO_EVENT_SOURCE * src)  {
  al_register_event_source(state->queue, src);
  return state;
}

/** Gets a color from the state' color list. NOT bound checked! */
ALLEGRO_COLOR state_color(State * state, int color) {
  return state->colors[color];
}

/** Sets a color to the state' color list. NOT bound checked! */
ALLEGRO_COLOR state_color_f(State * state, int color,
                         float r, float g, float b, float a) {
  return state->colors[color] = al_map_rgba_f(r, g, b, a);
}


/** Gets Ruby interpreter for state. */
Ruby * state_ruby(State * state) {
  return state->ruby;
}

/** Gets console intepreter for state. */
BBConsole * state_console(State * state) {
  return state->console;
}

/* Creates a new sprite in the sprite list. */
Sprite * state_new_sprite(State * state) {
  SpriteList * list = state_sprites(state);
  return spritelist_new_sprite(list);
}

/* Creates a new sprite in the sprite list and returns it's id. */
int state_new_sprite_id(State * state) {
  SpriteList * list = state_sprites(state);
  return spritelist_new_sprite_id(list);
}


/* Returns a sprite from the state's sprite list. */
Sprite * state_sprite(State * state, int index) {
  SpriteList * list = state_sprites(state);
  return spritelist_sprite(list, index);
}

/* Loads a layer of a sprite from a vpath. Sprite layer is in 
 ulpcss format. */
int state_sprite_load_builtin
(State * state, int sprite_index, int layer_index, char * vpath, int layout) { 

  return spritelist_load_sprite_layer_with_builtin_layout(
    state_sprites(state), sprite_index, layer_index, vpath, layout
  );
}
 
 
/* Looks up the thing by index and let the state's camera track it. 
 * If index is negative, stop tracking in stead.
 */
int state_camera_track_(State * state, int thing_index) {
  return -3;
}


/* Sets up the state's camera to track the numbered thing. */
int state_cameratrackthing(State * state, int thing_index) {
  return -1;
}

/* Lock in the state's camera to the current active tile map's given layer. 
 * Returns negative if no tile map is currently active. 
 */
int state_lockin_maplayer(State * state, int layer) {  
  return -1;
}


/* Loads a named tile map from the map folder. */
/* This function is obsolete. Load tile maps though script / store now. 
int state_loadtilemap_vpath(State * self, char * vpath) {
  TilemapLoadExtra extra;
  extra.area = self->area;
  self->loadingmap = fifi_load_vpath(tilemap_fifi_load, &extra, vpath);
  if(!self->loadingmap) return -1;
  tilemap_free(self->nowmap);
  self->nowmap = self->loadingmap;
  return 0;
}
*/

/* Sets the state current active actor to the thing with the given index.
 * Does not change if no such thing is found;
 */

/* Obsolete.  The scripts manage the active actor now. 
int state_actorindex_(State * self, int thing_index) {
  Thing * thing;
  thing = state_thing(self, thing_index);
  if(!thing) return -1;
  self->actor = thing;
  return thing_index;
}
*/

/* Returns the current active actor of the state. */
/* Obsolete.  The scripts manage the active actor now. 
Thing * state_actor(State * self) {
  if(!self) return NULL;
  return self->actor;
}
*/





#define STATE_MODES 10

int state_initjoystick(State * self) {
  int num, index, snum, sindex, bnum, bindex, anum, aindex;

  (void) self;
  if(!al_install_joystick()) return FALSE;
  num = al_get_num_joysticks();
  LOG_NOTE("Found %d joysticks:\n", num);
  for(index = 0; index < num; index ++) {
    ALLEGRO_JOYSTICK * joy = al_get_joystick(index);
    if(!al_get_joystick_active(joy)) continue;
    LOG_NOTE("Joystick nr %d, name: %s,", index, al_get_joystick_name(joy));
    snum = al_get_joystick_num_sticks(joy);
    LOG_NOTE("\n%d sticks: ", snum);
    for(sindex = 0; sindex < snum; sindex++) {
      LOG_NOTE("%s, ", al_get_joystick_stick_name(joy, sindex));
      anum = al_get_joystick_num_axes(joy, sindex); 
      LOG_NOTE("%d axes: ", anum);
      for (aindex = 0; aindex < anum; aindex++) {
        LOG_NOTE("%s, ", 
               al_get_joystick_axis_name(joy, sindex, aindex));
      }
    }
  
    bnum = al_get_joystick_num_buttons(joy);
    LOG_NOTE("\n%d buttons: ", bnum);
    for(bindex = 0; bindex < bnum; bindex++) {
      LOG_NOTE("%s, ", al_get_joystick_button_name(joy, bindex));
    }
    LOG_NOTE(".\n");
  }
  LOG_NOTE("\n");
  return num;
}



/** Initializes the state. It opens the screen, keyboards,
 interpreter, etc. Get any error with state_errmsg if
this returns NULL. */
State * state_init(State * self, BOOL fullscreen) {
  if(!self) return NULL;
  int flags        = 0;
  // initialize logging first
  if (!monolog_init()) {
    return state_errmsg_(self, "Could not init logging.");
  }

  // Initialize loggers
  monolog_add_logger(NULL, &state_stderr_logger);
  monolog_add_logger(fopen("eqk.log", "a"), &state_file_logger);
  // initialize log levels
  LOG_ENABLE_ERROR();
  LOG_ENABLE_WARNING();
  LOG_ENABLE_NOTE();
  LOG_NOTE("Starting logging", "");
  
  
  self->busy       = TRUE;
  self->fullscreen = fullscreen;
  self->audio      = FALSE;
  state_errmsg_(self, "OK!");
  // Initialize Ruby scripting 
  self->ruby = rh_new();
  if(!self->ruby) {
    return state_errmsg_(self, "Could not init Ruby.");
  }
  tr_init(self->ruby);
  
  // Initialize Allegro 5 and addons
  if (!al_init()) {
    return state_errmsg_(self, "Could not init Allegro.");
  }
  al_init_image_addon();
  al_init_font_addon();
  al_init_primitives_addon();
  if(!al_init_ttf_addon()) {
    return state_errmsg_(self, "Could not init TTF extension.\n");
  }

  // Install the keyboard handler
  if (!al_install_keyboard()) {
    return state_errmsg_(self, "Error installing keyboard.\n");
  }
  
  // install mouse handler   
  if (!al_install_mouse()) {
    return state_errmsg_(self, "Error installing mouse.\n");
  }
  
  // install joystick 
  if(!state_initjoystick(self)) {
    perror("Joysticks not started.");
  }

  /* Set up the audio system */
  self->audio = audio_init();
  if(!self->audio) {
    perror("Sound not started.");
  }

  // Use full screen mode if needed.
  if(self->fullscreen) { 
    flags = ALLEGRO_FULLSCREEN | ALLEGRO_GENERATE_EXPOSE_EVENTS;
  } else {
    /* flags = ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_GENERATE_EXPOSE_EVENTS; */
  }
  // flags |= ALLEGRO_OPENGL;
  
  /* Set up the usage of smoothing and of a depth buffer. */ 
  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
  
  // al_set_new_display_flags(ALLEGRO_RESIZABLE);
   
 
  al_set_new_display_flags(flags);
 /*  al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST); */
  // Create a window to display things on: 640x480 pixels.
  // self->display = al_create_display(1280, 960);
  
  
  
  self->display = al_create_display(SCREEN_W, SCREEN_H);
  if (!self->display) {
    return state_errmsg_(self, "Error creating display.\n");
  }
  
  
  
  // al_resize_display(self->display, SCREEN_W, SCREEN_H);
  
  // initialize the file finder, so we can start to load the data files
  if(!fifi_init()) { 
    return state_errmsg_(self, "Could not find data folder!\n");
  }
  
  // initialize the resource storage so we can store the data files
  if(!store_init()) { 
    return state_errmsg_(self, "Could not initialize data store.\n");
  }

  // Tuffy.ttf
  // "OpenBaskerville-0.0.53.otf"
  #define STATE_FONTNAME        "GranaPadano.ttf"
  #define STATE_FONT_INDEX      20000
  
  if(!store_load_ttf_font(STATE_FONT_INDEX, "font/" STATE_FONTNAME, -16, 0)) {
    return state_errmsg_(self, "Error loading " STATE_FONTNAME);
  }
  
  self->font = store_get_font(STATE_FONT_INDEX);
  
  // fifi_loadfont(STATE_FONTNAME, 16, 0);
  if (!self->font) {
    return state_errmsg_(self, "Error loading " STATE_FONTNAME);
  }

  state_color_f(self, STATE_WHITE, 1, 1, 1, 1);
  state_color_f(self, STATE_BLACK, 0, 0, 0, 1);
  // Start the event queue to handle keyboard input and our timer
  self->queue = al_create_event_queue();  
  state_eventsource(self, al_get_keyboard_event_source());
  state_eventsource(self, al_get_display_event_source(self->display));
  state_eventsource(self, al_get_mouse_event_source());
  state_eventsource(self, al_get_joystick_event_source());
  al_set_window_title(self->display, "Eruta!");
  // set up fps counter. Start with assuming we have 60 fps. 
  self->fps        = 60.0;
  self->fpstime    = al_get_time();
  self->frames     = 60;    
  /* No active maze yet. */
  /* state_active_maze_id_(self, -1); */
  
  /* Background color. */
  self->background_color = al_map_rgb(64,128,64);
  
  // set up camera
  self->camera = camera_new(vec3d(0, 0, -1), vec3d(0, 0, 1),
                            bevec(SCREEN_W, SCREEN_H), 45);
  
  if(!self->camera) {
      return state_errmsg_(self, "Out of memory when allocating camera.");
  }
  /* Set up console. */
  {
    Style   style = { color_rgb(255,255,255), color_rgba(64,0,0, 191), 
                      self->font, NULL};
    Rebox  bounds = { {20, 20} , {600, 400} }; 
    self->console = bbconsole_new(1, bounds, style);
    if(!self->console) {
      return state_errmsg_(self, "Out of memory when allocating console.");
    }
  }
  bbconsole_puts(self->console, "BBConsole started ok!");
  // set up ruby callback for console commands 
  bbconsole_command_(self->console, rh_run_console_command, self->ruby);

  // set up logging to console
  monolog_add_logger(self->console, &state_console_logger);

  
  /* Initialize sprite list. */
  self->sprites = spritelist_new();
  
  /* Show all by default. */
  self->show_area = TRUE;
  self->show_fps  = TRUE;
  self->show_graph= TRUE;
  
  
  
  
  return self;
}

/** Sets the state's busy status to false */
BOOL state_done(State * state) {
  state->busy = FALSE;
  return state->busy;
}

/** Returns true if the state is busy false if not. */
BOOL state_busy(State * self) {
  return self->busy;
}

/* Scales and moves the display to achieve resolution independence. */
void state_scale_display(State * self) {
   int real_w  = al_get_display_width(self->display);
   int real_h  = al_get_display_height(self->display);
   int scale_x = real_w / SCREEN_W;
   int scale_y = real_h / SCREEN_H;
   int scale   = (scale_x < scale_y) ? scale_x : scale_y;
   int offset_x= (real_w  - (SCREEN_W * scale)) / 2;
   int offset_y= (real_h  - (SCREEN_H * scale)) / 2;
   /*
   al_draw_textf(state_font(self), COLOR_WHITE,
                 100, 100, 0, "SCALE: w: %d, h: %d, sx: %d, sy: %d, s: %d, ox:%d, oy:%d",
                 real_w, real_h, scale_x, scale_y, scale, offset_x, offset_y);
    */
   ALLEGRO_TRANSFORM transform;
   al_identity_transform(&transform);
   /* Now draw black bars to cover the usused areas. */
   if (offset_y > 0) { 
    al_draw_filled_rectangle(-offset_x , -offset_y, real_w, 0, al_map_rgb(0,0,0));
    al_draw_filled_rectangle(-offset_x , SCREEN_H, real_w, SCREEN_H+offset_y, al_map_rgb(0,0,0));
   }
   if (offset_x > 0) { 
    al_draw_filled_rectangle(-offset_x , -offset_y, 0, real_h, al_map_rgb(0,0,0));
    al_draw_filled_rectangle(SCREEN_W  , -offset_y,  SCREEN_W + offset_x, real_h, al_map_rgb(0,0,0));
   }
   
   al_scale_transform(&transform, scale, scale);
   al_translate_transform(&transform, offset_x, offset_y);
   al_use_transform(&transform);
   /* al_set_clipping_rectangle(offset_x, offset_y, SCREEN_W, SCREEN_H); */
   
   
}

void draw_test_3d(void) {
  ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
  ALLEGRO_COLOR yellow = al_map_rgb(255, 255, 0);
  
  ALLEGRO_VERTEX p[] = { 
    {  -1,  -2,  0,  0, 0, red },
    {  -1,  -1,  0,  0, 0, red },
    {  -2,  -2,  0,  0, 0, red },
    {  0,  -4,  0,  0, 0, yellow },
    {  0,  -4,  -2,  0, 0, red   },
    {  -2,  -4,  -2,  0, 0, yellow  },
  };
    
  al_draw_prim(p, NULL, NULL, 0, 6, ALLEGRO_PRIM_TRIANGLE_LIST);
  // al_draw_filled_rectangle(1, 1, 2, 2, yellow);
  
}

void draw_test_2d(void) {
  ALLEGRO_COLOR blue = al_map_rgb(0, 0, 255);
  
  ALLEGRO_VERTEX p[3] = { 
    {  150,  250,  0,  0, 0, blue },
    {  150,  150,  0,  0, 0, blue },
    {  250,  250,  0,  0, 0, blue },
    
  };
    
  al_draw_prim(p, NULL, NULL, 0, 3, ALLEGRO_PRIM_TRIANGLE_LIST);
}



/* Draws all inside the state that needs to be drawn. */
void state_draw(State * self) {
  int layer;
  ALLEGRO_TRANSFORM normal_transform;
  ALLEGRO_TRANSFORM normal_view;
  ALLEGRO_TRANSFORM persp, camt;
  double dw, dh, f;
  dw = 640;
  dh = 480;
  f  = 1.0;

  normal_transform = *(al_get_current_projection_transform());
  normal_view      = *(al_get_current_transform());

  

  // camera_apply_perspective(state_camera(self), NULL);
  
  /* First set up perspective, then clear screen and depth buffer, and then 
   * move the camera. Finally draw the 3D scene.
   */
  al_identity_transform(&persp);
  al_translate_transform_3d(&persp, 0, 0, -5);
  
  //al_perspective_transform(&persp, -320, -240, 1, 320, 240, 10000);
  al_perspective_transform(&persp, (-1 * dw) / (dh * f), f , 1, 
                                    (f * dw) / dh      , -f, 10000);
  al_use_projection_transform(&persp);
  
  /* Draw background color. */
  al_clear_to_color(self->background_color);

  /* Use and clear depth buffer. */
  al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
  al_clear_depth_buffer(1.0);
  
  camera_apply_view(self->camera);

  
  draw_test_3d();
  draw_test_2d();

  /* Disable depth test for UI. */
  al_set_render_state(ALLEGRO_DEPTH_TEST, 0);
 
  
  al_identity_transform(&normal_transform);
  al_identity_transform(&normal_view);
  al_orthographic_transform(&normal_transform, 0, 0, -1, dw, dh, 10000);
   
  al_use_projection_transform(&normal_transform); 
  al_use_transform(&normal_view);
  
  // al_draw_filled_rectangle(0, 100, 200, 300, al_map_rgb(50, 100, 150));

  
  /* Draw 2D UI scene graph */
  if (self->show_graph) { 
    scegra_draw();
  }
  /* Draw the particles from the particle engine. */
  // alpsshower_draw(&shower, state_camera(state));
  // draw_test_2d();
  
  /* Draw fps if needed.  */
  if (self->show_fps) { 
    al_draw_textf(state_font(self), COLOR_WHITE,
                      10, 10, 0, "FPS: %.0f", state_fps(self));
                      
    al_draw_textf(state_font(self), COLOR_WHITE,
                      10, 20, 0, "Cam: %f %f %f %f %f", 
                      camera_at_x(self->camera),
                      camera_at_y(self->camera),
                      camera_at_z(self->camera),
                      camera_alpha(self->camera),
                      camera_theta(self->camera));
  } 
  
  /* Draw the console (will autohide if not active). */
  bbwidget_draw((BBWidget *)state_console(self));
  // state_scale_display(self); /* XXX: will this work for 3D projection??? */
}


/* Updates the state's display. */
void state_flip_display(State * self) {
  al_flip_display();
  state_frames_update(self);
}

/* Updates the state's elements. */
void state_update(State * self) { 
  mrb_value mval;
  // alpsshower_update(&shower, state_frametime(state));    
  
  camera_update(self->camera, state_frametime(self));
  // call ruby update callback 
  callrb_on_update(self);
  // Update the scene graph (after the Ruby upate so anty ruby side-changes take 
  // effect immediately.
  scegra_update(state_frametime(self));
  
}


/** Polls the state's event queue, and gets the next event and stores it in
* event. if it is available. Returns true if there is an event of false if
* not.
*/
int state_poll(State * state,  ALLEGRO_EVENT * event) {
  return al_get_next_event(state->queue, event);
}  

/** Polls the state's event queue, and gets the next event and returns it. 
* returns nul if out of memory or no event was available.
* You must free the result of this function with event_free
*/
ALLEGRO_EVENT * state_pollnew(State * state) {
  ALLEGRO_EVENT event, * result;
  
  if(state_poll(state, &event)) {
    result = event_alloc();
    if(!result) return NULL;
    (*result) = event; // copy data
    return result;     // return pointer
  }
  return NULL;
}


/** Return the state's default font. */
ALLEGRO_FONT * state_font(State * state) {
  if(!state) return NULL;
  return state->font;
}


/** Call this every frame to update the FPS and frames value */
void state_frames_update(State * state) {
  double now = al_get_time();
  state->frames++;

  if((now - state->fpstime) > 1.0) {
    double realfps;
     /* Measure only last second of frames, which means FPS gets updated every second or so. */
     realfps = ((double)state->frames) / (now - state->fpstime);
     /* Display and use a rounded value for FPS, the number after the comma is normally due to jitter anyway. */
     state->fps = floor(realfps + 0.5);

     /* A little trick, to prefent jerkyness, 
      * keep half the frames; and half the time  */
     state->frames  = state->frames / 2;
     state->fpstime = now - 0.5;
  } 
}

/** Returns the amount of frames rendered during this second. */
int state_frames(State * state) {
  return state->frames;
}

/** Returns the FPS value. */
double state_fps(State * state) {
  return state->fps;
}

/** Returns the Frame time value. */
double state_frametime(State * state) {
  if( state->fps < 20.0) return  1.0 / 20.0; 
  return 1.0 / state->fps;
}



/** Returns the camera of the state. */
Camera * state_camera(State * state) {
  if(!state) return NULL;
  return state->camera;
}

/* Get display state */
int global_state_show_fps() {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_fps; 
}

/* Set display state */
int global_state_show_fps_(int show) {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_fps = show;
}

/* Get display state */
int global_state_show_graph() {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_graph; 
}

/* Set display state */
int global_state_show_graph_(int show) {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_graph = show;
}

/* Get display state */
int global_state_show_area() {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_area; 
}

/* Set display state */
int global_state_show_area_(int show) {
  State * state = state_get();
  if (!state) return FALSE;
  return state->show_area = show;
}

/* Core functionality of Eruta, implemented on the state. */

/* Ideas about starting the game and progressing
* Normally, the game will have different modes. The game will start in 
* intro mode, which automatically changes into main menu mode. 
* From main menu mode a game may be loaded or a new game may be started. 
* 
* When the game starts, the generic startup and settings scripts must be loaded.
* They influence the design of the main menu. 
*  
* To begin, we must implement the new game start. When a new game is started,
* the game start script is loaded. Then, unless instructed differently, 
* map number 0001 is loaded. As always, when a map is loaded, 
* the map's corresponding script is loaded. The game has an area of "savable"
* data which is initialized either by loading the game or by the new game script.   
*  
* Before the game is saved, the save game script is loaded. Likewise before 
* the game is loaded the load game script is loaded. 
* 
* Once the game is running, the main game script is loaded and call-backs 
* are used to communicate with it. This is the mechanism that allows part of the game 
* to be implemented in script. 
* 
* Needed scripts so far: start.mrb, newgame.mrb, loadgame.mrb, savegame.mrb, 
* main.mrb, and one map_0001.mrb, etc for every map. Other scripts can be loaded by 
* these scripts for modulization, but only from the script directory.  
*  
*/



/* Preloads a tile map from the given vpath. Returns the loaded map, or 
 NULL if not loaded. 
Tilemap * state_preloadmap_vpath(State * state, const char * vpath) {
  
  return NULL;
}
*/ 

/* Preloads a tile map with the given map number. Returns the loaded map, or 
 NULL if not loaded. 
Tilemap * state_preloadmap_index(State * state, int index) {
  return NULL;
}
*/


/* Transforms a mask color of an image in storage into an alpha. */
int state_image_mask_to_alpha(State * state, int store_index, int r, int g, int b) {
  Image * image = store_get_bitmap(store_index);
  Color color   = al_map_rgb(r, g, b);
  (void) state;

  if (!image) return -1;  
  al_convert_mask_to_alpha(image, color);
  return store_index;
}



/* Transforms an image in storage where the average is assigned to the alpha value. */
int state_image_average_to_alpha(State * state, int store_index, int r, int g, int b) {
  Image * image = store_get_bitmap(store_index);
  Color color   = al_map_rgb(r, g, b);
  (void) state;
  if (!image) return -1;  
  draw_convert_average_to_alpha(image, color);
  return store_index;
}

/* Returns the first unused sprite ID that is greater than minimum. */
int state_get_unused_sprite_id() {
  return spritelist_get_unused_sprite_id(state_sprites(state_get()));
}

/** Deletes a sprite from the sprite list of the state. */
int state_delete_sprite(int index) {
  return spritelist_delete_sprite(state_sprites(state_get()), index);
}



