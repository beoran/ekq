#include <math.h>
#include "aldmg.h"

#define ALDMG_DEFAULT_SIZE 1024

typedef struct ALDMG_GUI ALDMG_GUI;


struct ALDMG_GUI {  
  ALDMG_WIDGET        * toplevel;   /* Toplevel widget. */
  size_t                space;      /* Amount of widget pointers in widgets */
  ALDMG_WIDGET       ** widgets;    /* Pointers to all widgets. */
  ALDMG_POINT           mouse;      /* Current mouse coordinates. */
  ALDMG_POINT           position;   /* Next widget position. */
  ALDMG_POINT           size;       /* Next widget position. */
  
  int                   click;      /* Recently clicked?. */
  
  int                   hover;      /* Widget ID currently hovered by mouse. */
  int                   marked;     /* Widget ID currently marked by keyjoy cursor. */  
  
  ALLEGRO_EVENT         last_event;   
  ALLEGRO_DISPLAY     * display;
  ALLEGRO_EVENT_QUEUE * queue;
  ALLEGRO_FONT        * builtin_font;
  double                time;
  ALDMG_STYLE           style;     /* Currently active style. */
};

/* Creates a widget for the given gui. */
ALDMG_WIDGET * aldmg_create_widget(ALDMG_GUI * gui) {
  ALDMG_WIDGET * widget = calloc(1, sizeof(ALDMG_WIDGET));
  widget->gui           = gui;  
  return widget;
}

/* Destroys a widget. */
void aldmg_destroy_widget(ALDMG_WIDGET * widget) {
  free(widget);
}


/** Adds more space for more widgets for this UI. Returns  negative if no memory is available. */
int aldmg_grow(ALDMG_GUI * gui) {
  int index;
  ALDMG_WIDGET ** aid;
  if (!gui) return -2;  
  aid = realloc(gui->widgets, (gui->space + ALDMG_DEFAULT_SIZE) * sizeof(ALDMG_WIDGET *));
  if (!aid) return -1;
  
  for (index = gui->space ; index < gui->space + ALDMG_DEFAULT_SIZE ; index ++) {
    aid[index] = NULL;
  }  
  gui->widgets = aid;  
  gui->space  += ALDMG_DEFAULT_SIZE;
  return 0;
}



/* Additional Drawing functions. */

/** Draws an image on the current target bitmap according to a 9 part scaling
algorithm. This splits the bitmap in 9 parts, keeps the 4 corners unscaled, but
scales the 4 edges and the center according to the desired size.
The 4 corners should be rectangles of identical size corner_w by corner_h in
the original image. Pass a non-positive number to have the corner sizes 
automatically calculated.
new_w and new_h are the new dimensions the new image should have.
This is useful for GUI backgrounds.
*/
void aldmg_blit_scale_9(ALLEGRO_BITMAP * img, int xx, int yy,
                                  int new_w, int new_h,
                                  int corner_w, int corner_h) {

  int mid_src_w, mid_src_h, mid_dst_w, mid_dst_h;
  int left_x, right_x, middle_x;
  int dst_y, src_y, part_w;
  int src_w = al_get_bitmap_width(img);
  int src_h = al_get_bitmap_height(img);
  corner_w  = (corner_w > 0) ? (corner_w) :  src_w / 16;
  corner_h  = (corner_h > 0) ? (corner_h) :  src_h / 16;
  CLAMP_MAX(corner_w, corner_w, (new_w / 4));
  CLAMP_MAX(corner_h, corner_h, (new_h / 4));
  mid_src_w = src_w - (corner_w * 2);
  mid_src_h = src_h - (corner_h * 2);
  mid_dst_w = new_w - (corner_w * 2);
  mid_dst_h = new_h - (corner_h * 2);
  // draw top parts first  
  left_x    = xx; // left side
  right_x   = xx    + mid_dst_w + corner_w; // right side
  middle_x  = xx    + corner_w ; // middle
  dst_y     = yy; // y is he drawing location for the top 3 draws
  // width of the first corner and middle. the second corner starts here
  part_w    = src_w - corner_w;
  // draw, take from the top corner left of the image
  al_draw_bitmap_region(img, 0, 0, corner_w, corner_h, left_x, dst_y, 0);
  // draw, take from the midle of the image
  al_draw_scaled_bitmap(img, corner_w, 0, mid_src_w, corner_h,
                  middle_x, dst_y, mid_dst_w, corner_h, 0);
  // draw, take from the right corner of the image
  al_draw_bitmap_region(img, part_w, 0, corner_w, corner_h, right_x, dst_y,  0);
  // now shift down, mutatis mutandis.
  dst_y     = yy + corner_h;
  src_y     = corner_h;
  // NOTE the middle left and right must be scaled, unlike the corners
  // that must be copied as they are.
  // draw, take from the middle corner left of the image, stretch vertically 
  al_draw_scaled_bitmap(img, 0, src_y, corner_w , mid_src_h,
                  left_x, dst_y, corner_w , mid_dst_h, 0);
  // draw, take from the midle center of the image
  al_draw_scaled_bitmap(img, corner_w, src_y, mid_src_w, mid_src_h,
                  middle_x, dst_y, mid_dst_w, mid_dst_h, 0);
  // draw, take from the right center of the image, stretch vertically
  al_draw_scaled_bitmap(img, part_w,  src_y, corner_w, mid_src_h,
                  right_x, dst_y, corner_w , mid_dst_h, 0);
  
  dst_y     = yy + new_h - corner_h ; // + mid_dst_h;
  src_y     = src_w - corner_h;
  // draw, take from the lower corner left of the image
  al_draw_bitmap_region(img, 0, src_y, corner_h, corner_w, left_x, dst_y, 0);
  // draw, take from the lower center of the image
  al_draw_scaled_bitmap(img, corner_w,  src_y, mid_src_w, corner_h,
                  middle_x, dst_y, mid_dst_w, corner_h, 0);
  
  // draw, take from the lower corner of the image
  al_draw_bitmap_region(img, part_w, src_y, corner_h, corner_w, right_x, dst_y,  0);
}


/*** Draws a filled rectange at the given position with the given size */
void aldmg_draw_slab(int x, int y, int w, int h, ALLEGRO_COLOR col) {
  al_draw_filled_rectangle(x, y, x+w, y+h, col);
} 

/*** Draws a rounded filled rectange at the given position with the given size */
void aldmg_draw_roundslab(int x, int y, int w, int h, int rx, int ry, ALLEGRO_COLOR col) {
  al_draw_filled_rounded_rectangle(x, y, x+w, y+h, rx, ry, col);
} 


/*** Draws an open rectange at the given position with the given size */
void aldmg_draw_box(int x, int y, int w, int h, ALLEGRO_COLOR col, int tt) {
  al_draw_rectangle(x, y, x+w, y+h, col, tt);
} 


/** Draws a rounded rectangle at the given position with the given size */
void aldmg_draw_roundbox(int x, int y, int w, int h, int rx, int ry, ALLEGRO_COLOR col, int tt) {
  al_draw_rounded_rectangle(x, y, x+w, y+h, rx, ry, col, tt);
} 
  

/** Draws a filled frame of the given thickness on the active bitmap.
* The outer size of the frame will be ww and hh.
* border color is fg, background color is bg. */
void aldmg_draw_frame(int xx, int yy, int ww, int hh, int tt, ALLEGRO_COLOR fg, ALLEGRO_COLOR bg)  {
  // Draw inner frame in bg color.
  aldmg_draw_slab(xx, yy, ww, hh, bg);
  // Draw outer frame in fg color with the given thickness.
  aldmg_draw_box(xx, yy, ww, hh, fg, tt);
}  
  
/** Draws a filled, rounded frame of the given thickness on the active bitmap.
* The rounding is autocalulated. The outer size of the frame will be ww and hh.
* border color is fg, background color is bg.
*/
void aldmg_draw_roundframe(int xx, int yy, int ww, int hh, int tt, ALLEGRO_COLOR fg, ALLEGRO_COLOR bg)  {
  int rx = 4;
  int ry = 4;
  // draw inner frame in bg color.
  aldmg_draw_roundslab(xx, yy, ww, hh, rx, ry, bg);
  // draw outer frame in fg color with the given thickness.
  aldmg_draw_roundbox(xx, yy, ww, hh, rx, ry, fg, tt);
}



/* Function that maps black to transparent black, white to transparent white, and 
 * any grayscale or color in between to white with an alpha value
 * that corresponds to the average of the r, g and b components. Mosty for use 
 * with the "GIN" icons. The color in draw color will be used for the r, g and b
 * of the color (premultiplied by the average as well)
 */
bool aldmg_convert_average_to_alpha(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR draw_color)
{
   ALLEGRO_LOCKED_REGION *lr;
   int x, y;
   float dr, dg, db;
   float avg, r, g, b;
   ALLEGRO_COLOR pixel;
   ALLEGRO_COLOR alpha_pixel;
   ALLEGRO_STATE state;

   if (!(lr = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ANY, 0))) {
      return FALSE;
   }

   al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
   al_set_target_bitmap(bitmap);

   alpha_pixel = al_map_rgba(0, 0, 0, 0);
   al_unmap_rgb_f(draw_color, &dr, &dg, &db);

   for (y = 0; y < al_get_bitmap_height(bitmap); y++) {
      for (x = 0; x < al_get_bitmap_width(bitmap); x++) {
         pixel = al_get_pixel(bitmap, x, y);
         al_unmap_rgb_f(pixel, &r, &g, &b);
         avg = (r + g + b) / 3.0;  
         alpha_pixel = al_map_rgba_f(dr * avg, dg * avg, db * avg, avg);
         al_put_pixel(x, y, alpha_pixel);
      }
   }

   al_unlock_bitmap(bitmap);

   al_restore_state(&state);
   return TRUE;
}



/** Calculates the brightness of a color on a scale of 0.0 to 1.0 */
float aldmg_get_color_brightness(ALLEGRO_COLOR color) {
  float r, g, b;
  al_unmap_rgb_f(color, &r, &g, &b);
  return (color.r + color.g + color.b) / 3.0;
}

/** Brighten a color by a given delta between 0 and 1. Alpha is not changed. */
ALLEGRO_COLOR aldmg_brighten_color(ALLEGRO_COLOR color, float delta) {
  float r, g, b, a;
  al_unmap_rgba_f(color, &r, &g, &b, &a);
  r *= 1 + delta; r =  (r > 1.0) ? 1.0 : r;
  g *= 1 + delta; g =  (g > 1.0) ? 1.0 : g;
  b *= 1 + delta; b =  (b > 1.0) ? 1.0 : b;
  if (r == 0.0) r = a * delta / 2.0;
  if (b == 0.0) b = a * delta / 2.0;
  if (g == 0.0) g = a * delta / 2.0;
  return al_map_rgba_f(r, g, b, a);
}

/** Darken a color by a given delta between 0 and 1. Alpha is not changed. */
ALLEGRO_COLOR aldmg_darken_color(ALLEGRO_COLOR color, float delta) {
  float r, g, b, a;
  al_unmap_rgba_f(color, &r, &g, &b, &a);
  r *= 1 - delta; r =  (r > 1.0) ? 1.0 : r;
  g *= 1 - delta; g =  (g > 1.0) ? 1.0 : g;
  b *= 1 - delta; b =  (b > 1.0) ? 1.0 : b;
  return al_map_rgba_f(r, g, b, a);
}

/** Creates a contrasting coolor by a given delta between 0 and 1, 
 * depending on whether thecolor is dark or light. */
ALLEGRO_COLOR aldmg_contrast_color(ALLEGRO_COLOR color, float delta) {
  if (aldmg_get_color_brightness(color) > 0.5) {
    return aldmg_darken_color(color, delta);
  } 
  return aldmg_brighten_color(color, delta);
}

/** Initializes a style */
ALDMG_STYLE aldmg_map_style(ALLEGRO_COLOR forecolor , ALLEGRO_COLOR backcolor,
                            ALLEGRO_FONT    * font  , ALLEGRO_BITMAP  * background, 
                            int         border_style, int               border_size,
                            float             margin,
                            float             grid_x,  float            grid_y) {

  ALDMG_STYLE result  = {{0}};
  result.forecolor    = forecolor;
  result.backcolor    = backcolor;          
  result.markcolor    = aldmg_contrast_color(backcolor, 0.5);        
  result.font         = font;
  result.background   = background;
  result.border_style = border_style;
  result.border_size  = border_size;
  result.margin       = margin;
  result.grid_x       = grid_x;
  result.grid_y       = grid_y;
  return result;
}


/* Creates a gui for a given display and event queue. */
ALDMG_GUI * aldmg_create_gui(ALLEGRO_DISPLAY * display, ALLEGRO_EVENT_QUEUE * queue) 
{
  ALDMG_GUI * gui = calloc(1, sizeof(ALDMG_GUI));
  if (!gui) return NULL;
  int w, h;
  w                 = al_get_display_width(display);
  h                 = al_get_display_height(display);
  gui->queue        = queue;
  gui->display      = display;
  gui->space        = 0;
  /* gui->widgets      = calloc(gui->space, sizeof(ALDMG_WIDGET *)); */
  aldmg_grow(gui);
  
  gui->builtin_font = al_create_builtin_font();
  gui->toplevel     = aldmg_create_widget(gui);  
  
  if ((!gui->toplevel) || (!gui->widgets) || (!gui->builtin_font)) {
    aldmg_destroy_gui(gui);
    return NULL;
  }
  
  gui->mouse.x      = 0;
  gui->mouse.y      = 0;
  gui->position.x   = 0;
  gui->position.y   = 0;
  gui->size.x       = 16.0;
  gui->size.y       = 16.0;
  gui->click        = 0;
  gui->style        = aldmg_map_style(al_map_rgb(255, 255, 255), al_map_rgba(0, 0, 128, 128),
                                      gui->builtin_font, NULL, ALDMG_BORDER_SOLID, 3, 
                                      2.0, w / 16, h / 16);
  return gui;
}

/* Destroys a GUI after use. */
void aldmg_destroy_gui(ALDMG_GUI * gui) 
{
  if (!gui) return;
    
  if (gui->builtin_font) { 
    al_destroy_font(gui->builtin_font);
  }
  
  if (gui->widgets) {
    int index;
    for (index = 0; index < gui->space; index++) {
      aldmg_destroy_widget(gui->widgets[index]);
      gui->widgets[index] = NULL;
    }
    free(gui->widgets);
    gui->widgets = NULL;
  }
    
  free(gui);  
}


/** Gets a widget for the given id. Returns NULL if no such widget. */
ALDMG_WIDGET * aldmg_get_widget(ALDMG_GUI * gui, int id) {
  if (!gui) return NULL;
  if (!gui->widgets) return NULL;
  if (id < 0) return NULL;
  if (id >= gui->space) return NULL;
  return gui->widgets[id];
}

/** Returns a free id for a widget. Returns negative if no space. */
int aldmg_get_id(ALDMG_GUI * gui) {
  int index;
  if (!gui) return -2;
  if (!gui->widgets) return -3;
  for (index = 0; index < gui->space ; index ++) {
    if (!gui->widgets[index]) return index;    
  }
  return -1;
}


/** Adds an existing widget to the gui with the given id. */
ALDMG_WIDGET * aldmg_put_widget(ALDMG_GUI * gui, int id, ALDMG_WIDGET * widget) {
  if (!gui) return NULL;  
  if (!gui->widgets) return NULL;
  if (id < 0) return NULL;
  if (id >= gui->space) return NULL;  
  gui->widgets[id] = widget;
  widget->id = id;
  return widget;
}

/** Adds a new widget to the GUI. ID is autogenerated and stored in id. */
ALDMG_WIDGET * aldmg_new_widget(ALDMG_GUI * gui, int id) {
  ALDMG_WIDGET * widget;
  if (id < 0) return NULL;  
  while (id >= gui->space)  {
    if (aldmg_grow(gui) < 0) return NULL;
  }  
  widget = aldmg_create_widget(gui);
  if (!widget) return NULL;  
  return aldmg_put_widget(gui, id, widget);  
}

/** Looks up the widget by id. If it doesn't exits yet, creates it. 
 * May still return null if creation failed.  */
ALDMG_WIDGET * aldmg_require_widget(ALDMG_GUI * gui, int id) {
  ALDMG_WIDGET * widget = aldmg_get_widget(gui, id);
  if (widget) return widget;
  return aldmg_new_widget(gui, id);
}


int aldmg_handle_key_down(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  return 0;
}

int aldmg_handle_key_up(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  return 0;
}

int aldmg_handle_key_char(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  return 0;
}

int aldmg_handle_mouse_button_down(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  gui->mouse.x = event->mouse.x;
  gui->mouse.y = event->mouse.y;  
  gui->click   = 1;
  return 1;
}

int aldmg_handle_mouse_button_up(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  gui->mouse.x = event->mouse.x;
  gui->mouse.y = event->mouse.y;  
  gui->click   = 0;
  return 1;
}

int aldmg_handle_mouse_axes(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  gui->mouse.x = event->mouse.x;
  gui->mouse.y = event->mouse.y;  
  gui->click   = 0;
  return 1;
}

int aldmg_handle_mouse_enter_display(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  gui->mouse.x = event->mouse.x;
  gui->mouse.y = event->mouse.y;  
  gui->click   = 0;  
  return 1;
}

int aldmg_handle_mouse_leave_display(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  gui->mouse.x = event->mouse.x;
  gui->mouse.y = event->mouse.y;  
  gui->click   = 0;
  return 1;
}

int aldmg_handle_mouse_warped(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  gui->mouse.x = event->mouse.x;
  gui->mouse.y = event->mouse.y;  
  gui->click   = 0;
  return 1;
}

int aldmg_handle_joystick_button_down(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  return 0;
}

int aldmg_handle_joystick_button_up(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  return 0;
}

int aldmg_handle_joystick_axes(ALDMG_GUI * gui, ALLEGRO_EVENT * event) {
  return 0;
}



/* Handles an incoming Allegro event. Returns zero if the event was not 
 * processed  by thegui, nonzero if it was and the rest of your program can 
 * ignore it if you like. 
 */
int aldmg_handle_event(ALDMG_GUI * gui, ALLEGRO_EVENT * event) 
{
  if (!event) return 0;
  gui->last_event = (*event);
  
  switch (event->type) {    
    case ALLEGRO_EVENT_KEY_DOWN:
      return aldmg_handle_key_down(gui, event);
      
    case ALLEGRO_EVENT_KEY_UP:
      return aldmg_handle_key_up(gui, event);
      
    case ALLEGRO_EVENT_KEY_CHAR:
      return aldmg_handle_key_char(gui, event);
                  
    case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
      return aldmg_handle_mouse_button_down(gui, event);
      
    case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
      return aldmg_handle_mouse_button_up(gui, event);      
      
    case ALLEGRO_EVENT_MOUSE_AXES:
      return aldmg_handle_mouse_axes(gui, event);
      
    case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
      return aldmg_handle_mouse_enter_display(gui, event);
      
    case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
      return aldmg_handle_mouse_leave_display(gui, event);
      
    case ALLEGRO_EVENT_MOUSE_WARPED:
      return aldmg_handle_mouse_warped(gui, event);      
      
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
      return aldmg_handle_joystick_button_down(gui, event);
      
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
      return aldmg_handle_joystick_button_up(gui, event);      
      
    case ALLEGRO_EVENT_JOYSTICK_AXIS:
      return aldmg_handle_joystick_axes(gui, event);
          
    default: /* Unrecognised or non-gui related event. */
      return 0;      
  }
  
  return 0;
}

/* Updates the timer of the gui to do animations, etc. */
void aldmg_update_gui(ALDMG_GUI * gui, double dt)
{
    gui->time += dt;
}

/* Draws  the GUI. */
void aldmg_draw_gui(ALDMG_GUI * gui) 
{
  
}

/* Returns a pointer to the toplevel widget of the GUI. This always exists, 
 * but normally simply does nothing but draw it's background if set. The id 
 * of the toplevel widget is 0. 
 */
ALDMG_WIDGET * aldmg_get_toplevel_widget_from_gui(ALDMG_GUI * gui);


/* Returns a box with the given position and size. */
ALDMG_BOX aldmg_box(float x, float y, float w, float h);

/* Returns a default style for the GUI. 
 * The font will be an instance of Allegro's built-in font. */
ALDMG_STYLE aldmg_get_default_style(ALDMG_GUI * gui);

/** Indicates the beginning of the gui handling and setup. */
int aldmg_start_gui(ALDMG_GUI * gui) {
  return 0;
}

/** Indicates the end of the gui handling and setup. */
int aldmg_end_gui(ALDMG_GUI * gui) {
  return 0;
}

/* Adds and handles a custom widget to the GUI. */
int aldmg_custom_widget(ALDMG_GUI * gui, ALDMG_WIDGET * widget, ...);

/* Adds and handles a custom widget to the GUI. */
int aldmg_custom_widget_va(ALDMG_GUI * gui, ALDMG_WIDGET * widget, va_list args);

/* Indicates the end of a container-style custom widget. 
 * Not needed for non-containers. */
int aldmg_end_custom_widget(ALDMG_GUI * gui, ALDMG_WIDGET * widget);


/* Indicates the end of a container-style built in widget. 
 * Not needed for non-containers. */
int aldmg_end_widget(ALDMG_GUI * gui, int id);

/* Adds a logical GUI page to the GUI and/or handles it's state. */  
int aldmg_page(ALDMG_GUI * gui, int id, ...);

/* Adds a logical GUI page to the GUI and/or handles it's state. */  
int aldmg_page_va(ALDMG_GUI * gui, int id, va_list args);

/* Adds a frame to the GUI and/or handles it's state. */  
int aldmg_frame(ALDMG_GUI * gui, int id, ...);

int aldmg_mouse_inside(ALDMG_GUI * gui, int x, int y, int w, int h) {
  return (gui->mouse.x >= x) 
      && (gui->mouse.y >= y) 
      && (gui->mouse.x <= (x+w))
      && (gui->mouse.y <= (y+h));
}

  
float aldmg_y_percentage_to_real(ALDMG_GUI * gui, float ydim) {
  float dh = al_get_display_height(gui->display);
  return round((dh * ydim) / 8.0) * 8.0;
}

  
float aldmg_x_percentage_to_real(ALDMG_GUI * gui, float xdim) {
  float dw = al_get_display_width(gui->display);
  return round((dw * xdim) / 8.0) * 8.0;  
}


/* Sets the position for the next widget. */
ALDMG_GUI * aldmg_set_position(ALDMG_GUI * gui, float px, float py) {
  gui->position.x = px;
  gui->position.y = py;
  return gui;
}

/* Sets the style for the next widget. */
ALDMG_GUI * aldmg_set_style(ALDMG_GUI * gui, ALDMG_STYLE style);

/* Sets the size for the next widget. GUI is returned for chaining.*/
ALDMG_GUI * aldmg_set_size(ALDMG_GUI * gui, float pw, float ph) {
  gui->size.x = pw;
  gui->size.y = ph;
  return gui;
}

/* Gets the currrent draw position and size as a bounds box.
 */
ALDMG_BOX aldmg_get_current_bounds(ALDMG_GUI * gui) {
  ALDMG_BOX box;
  box.x    = gui->position.x;  
  box.y    = gui->position.y;
  box.w    = gui->size.x;
  box.h    = gui->size.y;
  return box;
}

/* Advances the current position based on the currend size 
 * of the widget box and the size of the parent. 
 */
int aldmg_advance_position(ALDMG_GUI * gui) {
  gui->position.x    += gui->size.x;  
  if (gui->position.x > al_get_display_width(gui->display)) {
     gui->position.x  = 0;
     gui->position.y += gui->size.y;
  }
  return 1;
}

/* Draws a label with variable formating. */
int aldmg_vlabelf(ALDMG_GUI * gui, int id, char * format, va_list args) {
  char buffer[1024];
  int m = 2;
  ALDMG_BOX b = aldmg_get_current_bounds(gui);  
  aldmg_advance_position(gui);  
  
  vsnprintf(buffer, sizeof(buffer), format, args);
  al_draw_text(gui->style.font, gui->style.forecolor, b.x + m , b.y + m, 0, buffer);
  return 1;  
}

/** Draws a label with variable formatting. */
int aldmg_labelf(ALDMG_GUI * gui, int id, char * format, ...) {
  int result;
  va_list args;
  va_start(args, format);
  result = aldmg_vlabelf(gui, id, format, args);
  va_end(args);
  return result;  
}


/* Rectangular frame. Can have an image as a background. */

/* Polygonal frame. */

/* Round rectangle frame. */

/* Round rectangle frame. */


/* Adds a button the the GUI and/or handles it's state. */  
int aldmg_button(ALDMG_GUI * gui, int id, char * label) {
  ALDMG_WIDGET * widget = aldmg_require_widget(gui, id);
  int m = 2;
  int inside;
  ALDMG_BOX b = aldmg_get_current_bounds(gui);  
  aldmg_advance_position(gui);      
  inside = aldmg_mouse_inside(gui, b.x, b.y, b.w, b.h);
  if (inside) { 
    al_draw_filled_rounded_rectangle(b.x, b.y, b.x + b.w, b.y + b.h, 2, 2, gui->style.markcolor);    
  } else {
    al_draw_filled_rounded_rectangle(b.x, b.y, b.x + b.w, b.y + b.h, 2, 2, gui->style.backcolor);
  }
  al_draw_rounded_rectangle(b.x, b.y, b.x + b.w, b.y + b.h, 2, 2, gui->style.forecolor, 2);
  al_draw_textf(gui->style.font, gui->style.forecolor, b.x + m , b.y + m, 0, "%s", label);
  if (inside && gui->click) {
    gui->click = 0;
    return 1;
  }
  
  return 0;
}


/* Adds a horizontal bar based on a stat and a maximum. */
int aldmg_hbar(ALDMG_GUI * gui, int id, double stat, double max, ALLEGRO_COLOR color) {  
  double m = 2;
  double fw =  stat / max;
  ALDMG_BOX b = aldmg_get_current_bounds(gui);
  aldmg_advance_position(gui);
  al_draw_filled_rectangle(b.x + m, b.y + m, b.x + (fw * b.w) - m, b.y + b.h - m, color);
  al_draw_rectangle(b.x + m, b.y + m , b.x + b.w - m, b.y + b.h - m, gui->style.forecolor, 1);
  return 0;
}



/* Adds a vertical menu to the GUI and/or handles it's state. */  
int aldmg_vmenu(ALDMG_GUI * gui, int id, ...);

/* Adds a menu item to the GUI and/or handles it's state. */  
int aldmg_item(ALDMG_GUI * gui, int id, ...);


