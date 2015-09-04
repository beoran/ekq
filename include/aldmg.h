#ifndef ALUM_H_INCLUDED
#define ALUM_H_INCLUDED

#include <stdlib.h>
#include <stdarg.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "bad.h"

/* Forward typedefs. */

typedef struct ALDMG_GUI                ALDMG_GUI;
typedef struct ALDMG_POINT              ALDMG_POINT;
typedef struct ALDMG_BOX                ALDMG_BOX;
typedef struct ALDMG_STYLE              ALDMG_STYLE;
typedef struct ALDMG_WIDGET             ALDMG_WIDGET;



/*
*
* Represents a point in 2D space, or a 2D size.
*/
struct ALDMG_POINT {
  float x, y;
};

/*
*
* Represents a rectangular box. Note that in ALDMG, sizes and positions 
* of widgets are often expressed as relative to the container of the 
* widget or to the screen for a toplevel widget.
*/
struct ALDMG_BOX {
  float x, y, w, h;
};


/*
* Describes the style of a widget.
*/
struct ALDMG_STYLE {
  ALLEGRO_COLOR     forecolor;
  ALLEGRO_COLOR     backcolor;
  ALLEGRO_COLOR     markcolor;
  ALLEGRO_FONT    * font;
  ALLEGRO_BITMAP  * background;
  ALLEGRO_BITMAP  * marker;
  
  int               border_style;
  int               border_size;
  float             margin;
  float             grid_x;
  float             grid_y;
};


/* A widget, useful for defining custom widgets.
 */
struct ALDMG_WIDGET {
  ALDMG_GUI   * gui;
  ALDMG_STYLE   style;
  ALDMG_BOX     relative;
  ALDMG_BOX     absolute;
  int           id;
  int           parent;
  int           sibling;
  int           child;
  int           flags;
  void        * data;
  size_t      * size;
  void        (*draw)(ALDMG_WIDGET *);
  void        (*update)(ALDMG_WIDGET *);
  int         (*handle)(ALDMG_WIDGET *);
  int         (*setup)(char * key, va_list args);
};
 
/** Enum for the */ 
 
/* An enum that contains ALDMG related constants. */ 
enum ALDMG_CONSTANTS {
  ALDMG_VGINPUT_AXES    = 8,
  ALDMG_VGINPUT_BUTTONS = 32, 
  ALDMG_BORDER_NONE     = 0,
  ALDMG_BORDER_SOLID    = 1,
};

/** A virtual game input device. */
struct ALDMG_VGINPUT {

};


/* Configuration of a GUI. This includes mappings for keys, joystick
 * buttons and joystick axes. These are mapped to a virtual game
 * input device that has directions, buttons and triggers.
 */ 
 
 
 
/* Creates a gui for a given display and event queue. */
ALDMG_GUI * aldmg_create_gui(
    ALLEGRO_DISPLAY * display, ALLEGRO_EVENT_QUEUE * queue);

/* Destroys a GUI after use. */
void aldmg_destroy_gui(ALDMG_GUI * gui);

/* Handles an incoming Allegro event. */
int aldmg_handle_event(ALDMG_GUI * gui, ALLEGRO_EVENT * event);

/* Updates the timer of the gui to do animations, etc. */
void aldmg_update_gui(ALDMG_GUI * gui, double dt);

/* Draws  the GUI. */
void aldmg_draw_gui(ALDMG_GUI * gui);  

/* Returns a free ID for a widget for the GUI. */
int aldmg_get_id(ALDMG_GUI * gui);


/* Returns a pointer to the toplevel widget of the GUI. This always exists, 
 * but normally simply does nothing but draw it's background if set. The id 
 * of the toplevel widget is 0. 
 */
ALDMG_WIDGET * aldmg_get_toplevel_widget_from_gui(ALDMG_GUI * gui);

/* Gets a widget from this GUI by it's ID. */
ALDMG_WIDGET * aldmg_get_widget_from_gui(ALDMG_GUI * gui, int id);

/* Returns a box with the given position and size. */
ALDMG_BOX aldmg_box(float x, float y, float w, float h);

/* Returns a default style for the GUI. 
 * The font will be an instane of Allegro's built-in font. */
ALDMG_STYLE aldmg_get_default_style(ALDMG_GUI * gui);

/** Indicates the beginning of the gui handling and setup. */
int aldmg_start_gui(ALDMG_GUI * gui);

/** Indicates the end of the gui handling and setup. */
int aldmg_end_gui(ALDMG_GUI * gui);

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


/* Sets the position fo the next widget. */
ALDMG_GUI * aldmg_set_position(ALDMG_GUI * gui, float py, float px);

/* Sets the style for the next widget. */
ALDMG_GUI * aldmg_set_style(ALDMG_GUI * gui, ALDMG_STYLE style);

/* Sets the size for the next widget. GUI is returned for chaining.*/
ALDMG_GUI * aldmg_set_size(ALDMG_GUI * gui, float pw, float ph);



/* Draws a label with variable formating. */
int aldmg_vlabelf(ALDMG_GUI * gui, int id, char * format, va_list args);

/** Draws a label with variable formatting. */
int aldmg_labelf(ALDMG_GUI * gui, int id, char * format, ...);

/* Adds a button the the GUI and/or handles it's state. */
int aldmg_button(ALDMG_GUI * gui, int id, char * label);

/* Adds a vertical menu to the GUI and/or handles it's state. */  
int aldmg_vmenu(ALDMG_GUI * gui, int id, ...);

/* Adds a menu item to the GUI and/or handles it's state. */  
int aldmg_item(ALDMG_GUI * gui, int id, ...);


int aldmg_hbar(ALDMG_GUI * gui, int id, double stat, double max, ALLEGRO_COLOR color);



/* Utility macros */

/** Clamp an integer VALUE to be less than MAX and store it in SET. */
#define CLAMP_MAX(SET, VALUE, MAX) do {                     \
  int value___ = (VALUE), max___ = (MAX);                   \
  (SET) =  ((value___ > max___ ) ? (max___) : (value___));  \
} while(0) 

#endif // ALUM_H_INCLUDED
