/**
* @mainpage Gari Game Library
*
* Gari is a simple, fast, cross platform game library targeted at 2D games,
* written in plain C. For now, it uses SDL as a backend, but that might 
* change later to use more low-level back-ends. It's goals are:
*
* 1) Targeted to make writing 2D games as easy as possible.
* 2) A simple API, that hides the details as much as possible.
* 3) Fast, giving good performance everywhere possible.
* 4) Cross platform. Enough said.
* 5) Complete yet concise. All you need to write 2D games, but nothing more.
* 6) Release under Zlib license (or similar permissive).
* 7) Easy to embed in Ruby with bindings provided.
*
* A C compiler which supports some C99 fratures, particularly
* stdint.h, __VA_ARGS__, and one line comments is needed to compile Gari.
*
* More details are documented in gari.h
* 
* @file gari.h
*
*/

#ifndef _GARI_H_
#define _GARI_H_

#include <stdlib.h>
#include <stdint.h>

/* 
  Some platforms will need a __declspec(dllexport) or something for 
  functions in a shared library. Not used yet since I didn't get a chance to try it there yet.
*/
#ifndef GARI_EXPORT_FUNC
#define GARI_EXPORT_FUNC extern
#endif
  
/*
* Some platforms may require a change in calling convention
*/  
#ifndef GARI_CALL_FUNC
#define GARI_CALL_FUNC
#endif 

/* All in a handy wrapper macro */
#define GARI_FUNC(RESULT) GARI_EXPORT_FUNC RESULT GARI_CALL_FUNC
 
/* An another wrapper macro to help typedefing such functions */
#define GARI_FUNCTYPE(RESULT) typedef RESULT GARI_CALL_FUNC


/** Default color depth to open the creen with. Normally 32.  */
#ifndef GARI_DEFAULT_DEPTH 
#define GARI_DEFAULT_DEPTH 32
#endif

/** Of course we need fake booleans if we don' talready have them :p. */
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

/** Common alpha values */
#define GARI_ALPHA_OPAQUE      255
#define GARI_ALPHA_SOLID       255
#define GARI_ALPHA_HALF        128
#define GARI_ALPHA_CLEAR       0
#define GARI_ALPHA_TRANSLUCENT 0


/** Dye. A Dye is a device and image dependent representation of a color 
    that fits inside a uint32_t types.
    Dyes are used in lower level API's. 
*/
typedef uint32_t GariDye;

/** GariColor models colors as RBGA values in an sRGBA color space.
* GariColor should be used as if it is immutable.
* Colors are used in stead of Dyes in higher level API's.  
*/
struct GariColor_ {
  uint8_t r, g, b, a;
};

typedef struct GariColor_ GariColor;

/** Frees the memory associated with a GariColor allocated though
*   gari_color_rgba or gari_color_rgb.  Returns NULL (guaranteed).
*/
GariColor * gari_color_free(GariColor * color); 

/** Allocates a new color and initializes it with the given r, g, b and a values
*/
GariColor * gari_color_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/** Allocates a new color and initializes it with the given r, g, b values
*   a will be set to GARI_ALPHA_OPAQUE.
*/ 
GariColor * gari_color_rgb(uint8_t r, uint8_t g, uint8_t b);  


/** Returns a GariColor struct initialized with the given 
* r, g, b, a components.
*/
GariColor gari_colora(uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/** Returns a GariColor struct initialized with the given r, g, b components. 
* The a component will be set to GARI_ALPHA_SOLID.
*/
GariColor gari_color(uint8_t r, uint8_t g, uint8_t b);

/** Gets the r component of a GariColor. */
uint8_t gari_color_r(GariColor * rgba);

/** Gets the g component of a GariColor. */
uint8_t gari_color_g(GariColor * rgba);

/** Gets the b component of a GariColor. */
uint8_t gari_color_b(GariColor * rgba);

/** Gets the a component of a GariColor. */
uint8_t gari_color_a(GariColor * rgba);


/** Alpha Levels. Alpha Levels are models as simple uint8_t types. */
typedef uint8_t GariAlpha;

/** Images. Images arevisuals that can be dwawn to. */
struct GariImage_; 
typedef struct GariImage_ GariImage;

/** A screen or window. Not directly drawable. */
struct GariScreen_; 
typedef struct GariScreen_ GariScreen;

/** Game info. */
struct GariGame_; 
typedef struct GariGame_ GariGame;

/** Drawing information */
struct GariDraw_; 
typedef struct GariDraw_ GariDraw;




/** Drawing callback function. */
typedef int GariDrawFunction(GariDraw * data, int px, int py); 



/** 
* Initializes Gari Game and returns it. Must be done before anything else. 
*/
GariGame * gari_game_make();

/** Finishes and cleans up a gari game when we're done with it. */
void gari_game_free(GariGame * game);

/** Updates the game screen, FPS, etc. */
GariGame * gari_game_update(GariGame * game); 

/** Sets the frames of the game back to 0.*/
GariGame * gari_game_resetframes(GariGame * game); 

/** Gets the total amount of frames displayed since the start of the game, 
   or since gari_game_resetframes was called. */
uint32_t gari_game_frames(GariGame * game);

/** Advances the game's frame counter. */
GariGame * gari_game_nextframe(GariGame * game);

/** Starts FPS counter. gari_game_resetframes also calls this. */
GariGame * gari_game_startfps(GariGame * game);

/** Returns calculated fps after calling startfps . */
double gari_game_fps(GariGame * game);

/** Reports on the game's FPS on stderr.*/
void gari_game_report(GariGame * game);


/** Opens the game main game screen or window. Must be done before using any image functions, or before calling game.update . */
GariScreen * gari_game_openscreendepth(GariGame * game, int wide, int high, int fullscreen, int depth); 

/** Opens the game main game screen or window. 
   Must be done before using any image functions. */   
GariScreen * gari_game_openscreen(GariGame * game, int wide, int high, 
                                  int fullscreen);

// Checks if a screen is the fullscreen screen or not
int gari_screen_fullscreen(GariScreen * screen);

/** Retuns the current main game screen. */
GariScreen * gari_game_screen(GariGame * game);

/** Returns the drawable image for the screen. */
GariImage * gari_screen_image(GariScreen * screen);

/** Checks if the main game screen is in fullscreen mode. */ 
int gari_game_fullscreen(GariGame * game);

/** Can be used to set or unset fullscreen after opening the screen. */
GariScreen * gari_game_fullscreen_(GariGame * game, int fullscreen); 

/** Sets the keyboard repeat delay and interval in ms, 
*   or disable with delay 0. */
GariGame * gari_game_keyrepeat(GariGame * game, int delay, int interval);

/** Sets the system mouse cursor to invisible or visible. */
int gari_screen_showcursor_(GariScreen * screen, int show);
/** Gets the visibility of the system mouse cursor. */
int gari_screen_showcursor(GariScreen * screen);


/** Image mode, this deterines how to optimize the image for drawing to the screen, and whether the image has any transparency. */
enum GariImageMode_ { 
  GariImageSolid      = 0,
  GariImageColorkey   = 1,
  GariImageAlpha      = 2
};


/** Makes an empty gari image with given bits per pixels (in depth), 
    but supporting either alpha, or not, depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gari_image_free when you're done with the generated 
    GariImage. May return NULL on error.
*/
GariImage * gari_image_makedepth(int w, int h, int depth, int mode);


/** Makes an empty gari image with the same bits per pixels as the screen, 
    but supporting either alpha, or not, or a colorkey depending on mode.
    The video mode must have been set and the screen must have been opened.
    You must call gari_image_free when you're done with the generated 
    GariImage. May return NULL on error.
*/
GariImage * gari_image_make(int w, int h, int mode, GariDye dyekey);

/** Disposes of an image. */
void gari_image_free(GariImage * image);

/** Image loading functions. */

/** Loads the image from the named file. */
GariImage * gari_image_loadraw(char * filename);


/** Loads the image from the named file, and optimizes it for display
as a solid image on the active video surface, which must already have been 
opened. If the acceleration fails, this returns the unaccellerated image. 
Return NULL if the file could not be read. */
GariImage * gari_image_loadsolid(char * filename);

/** Loads the image from the named file and tries to optimize it for display on the game's screen. This requires the game's screen to have been opened first. */
GariImage * gari_image_load(GariGame * game, char * filename);


/*/ Saves a GariImage as a to a file in filename in BMP format. Return NULL if 
saving failed, img on success. */
GariImage * gari_image_savebmp(GariImage * img, const char * filename);

/** Saves a GariImage as a to a file in filename in PNG format. Return NULL if 
saving failed, img on success. */
GariImage * gari_image_savepng(GariImage * img, const char * filename);

/** Optimizes the image for drawing to the screen. */
GariImage * gari_image_optimize(GariImage * image, int mode, GariDye dyekey);

/** Copies a part of the source image and returns a new destination image,
or nil it it ran out of memory or otherwise was incorrect. */
GariImage * gari_image_copypart(GariImage *src, int x, int y, int w, int h);

/** Returns true if any pixel of the image has any per-pixel transparency, 
lower than amin 
false if not. */
int gari_image_hasalpha_p(GariImage * img, unsigned char amin);

/** Returns the width of the image. */
int gari_image_w(GariImage * img);
 
/** Returns the height of the image. */
int gari_image_h(GariImage * img);

/** Gets the color depth in bits per pixels of this image. Returns -1 on error.
*/
int gari_image_depth(GariImage * img);

/** Locks image before drawing. Needed before any _nolock drawing function. */
void gari_image_lock(GariImage * image); 
/** Unlocks image after drawing. Needed after any _nolock drawing function. */
void gari_image_unlock(GariImage * image); 

/** Converts a color expressed in rgb components to a GariDye. */
GariDye gari_image_rgb(GariImage *img, uint8_t r, uint8_t g, uint8_t b);

/** Converts a color expressed in rgba components to a GariDye. */
GariDye gari_image_rgba(GariImage *img, 
          uint8_t r, uint8_t g, uint8_t b, uint8_t a);


          
/** 
  Concstructs a color expressed as an RGBA quadruplet and returns it as a 
  struct.
*/
GariColor gari_colora(uint8_t r, uint8_t g, uint8_t b, uint8_t a); 

GariColor gari_color(uint8_t r, uint8_t g, uint8_t b);

/** 
* Converts a GariColor to a gari dye for the given image. 
* If the color's A is solid, then it maps as a solid color. 
* otherwise, it maps as a transparent color.  
*/
GariDye gari_color_dye(GariColor color, GariImage * image); 

/** Checks if two gariColors are equal. Colors will sort by ascending a,
r, g and b */
int gari_color_cmp(GariColor c1, GariColor c2);

/** Converts a GariDye to a GariColor for the given image. */
GariColor gari_dye_color(GariDye dye, GariImage * image);

/** Optimizes a color for use with the given image. For 24 or 32 bpp images 
this does nothing. However, other image depths, such as 16, 15 or 8 bits don't have enough resolution to display 3 or 4 rgb(a) components correctly, so the 
colors used are reduced. All in all, this fuction is a round-trip between 
gari_color_dye and gari_dye_color.  
*/
GariColor gari_color_optimize(GariColor color, GariImage * image);
         

/** Drawing functions. */
typedef void GariPutPixelFunc(GariImage * img, int x, int y, GariDye dye);
typedef GariDye GariGetPixelFunc(GariImage * img, int x, int y);
typedef void GariBlendPixelFunc(GariImage * img, int x, int y, GariDye dye, GariAlpha alpha);

/** Returns a function that can be used to put a pixel on this GariImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.    
*/
GariPutPixelFunc * gari_image_putpixelfunc_nl(GariImage * image);

/** Returns a function that can be used to get a pixel from this GariImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.
*/    
GariGetPixelFunc * gari_image_getpixelfunc_nl(GariImage * image); 

/** Returns a function that can be used to blend a pixel from this GariImage.
* The function returned does no locking. May return NULL if the format does 
* not use 1,2,3 or 4 bytes per pixel.
*/    
GariBlendPixelFunc * gari_image_blendpixelfunc_nl(GariImage * image); 


/* Drawing context, used for abstracting certain drawing functions.*/
struct GariDraw_ {
  GariImage        *  image; // image we're drawing to. 
  GariDrawFunction *  draw;  // Generic drawing function to use.
  GariGetPixelFunc *  getpixel; // Get pixel function (for the image) 
  GariPutPixelFunc *  putpixel; // Put pixel function (for the image)
  GariBlendPixelFunc *blendpixel; // Blend pixel function (for the image)
  GariColor           color; // Color to draw with.
   GariDye             dye;   // Dye to draw with.
  GariAlpha           alpha; // alpha value to be used when drawing
  void *              other; // other user-defined data.
};

/* Constructs drawing information */
GariDraw * gari_drawdata_init(
                              GariDraw          * data,
                              GariImage         * image,
                              GariDrawFunction  * func,
                              GariColor color,
                              GariAlpha alpha
                             ); 
                             
/** Sets the clipping rectangle of the image. 
* Clipping applies to all drawing functions. 
*/
GariImage * gari_image_setclip(GariImage * img, 
              int x, int y, int w, int h); 
  
/** Gets the clipping rectangle of the image. 
* Clipping applies to all drawing functions. 
*/
GariImage * gari_image_getclip(GariImage * img, 
              int *x, int *y, int *w, int *h);

/** Quickly fills the image or screen with the given dye */
void gari_image_filldye(GariImage * image,  GariDye dye);
 
/** Quickly draws a rectangle with the given dye, without blending. */
void gari_image_fillrectdye(GariImage * image, int x, int y, 
                          int w, int h, GariDye dye);
                          
/** Quickly fills the image with the given color. */
void gari_image_fill(GariImage * image,  GariColor color);

/** Quickly draws a rectangle with the given dye, without blending. */
void gari_image_fillrect(GariImage * image, int x, int y, 
                          int w, int h, GariColor color); 
                          

/** Low level putpixel function.         */
void gari_image_putpixel(GariImage * image, int x, int y, GariDye dye);

/** High level level putpixel function.  */
void gari_image_dot(GariImage * image, int x, int y, GariColor color);

/** High level blend pixel function */
void gari_image_blenddot(GariImage * image, int x, int y, GariColor color);

/** High level getpixel function */
GariColor gari_image_getdot(GariImage * image, int x, int y);

/** Draws an arbitrary line on the image starting at x1, y1, and fitting inside
the rectangle with the size w and h.     */
void gari_image_line(GariImage * image, int x1, int y1, 
                     int w, int h, GariColor color);

/** Optimized case of gari_image_line, draws horizontal lines with 
    a width of w only. */
void gari_image_hline(GariImage * image, int x, int y, int w, GariColor color);

/** Optimized case of gari_image_line, draws vertical lines with 
   a height h only. */
void gari_image_vline(GariImage * image, int x, int y, int h, GariColor color); 

/** Draws a hoop, that is, an empty circle, on the image. */
void gari_image_hoop(GariImage * image, int x, int y, int r, GariColor color);

/** Draws a disk, or a filled circle, on the image. */
void gari_image_disk(GariImage * image, int x, int y, int r, GariColor color);

/** Draws a box, or an open rectangle on the image, starting at (x, y) */
void gari_image_box(GariImage * image, int x, int y, 
                                       int w, int h, GariColor color);

/** Draws a slab, which is a filled rectange, on the image. */
void gari_image_slab(GariImage * image, int x, int y, 
                      int w, int h, GariColor color);

/** Blends an arbitrary line on the image starting at x1, y1, and fitting inside
the rectangle with the size w and h.     */
void gari_image_blendline(GariImage * image, int x1, int y1, 
                     int w, int h, GariColor color);
                     
/** Blends a box, which is an open rectange, on the image. */
void gari_image_blendbox( GariImage * image, int x, int y, int w, int h,  
                      GariColor color);                     

/** Draws a blended slab, which is a filled rectange, on the image. */
void gari_image_blendslab( GariImage * image, int x, int y, int w, int h,  
                      GariColor color);

/** Draws a blended disk, which is a filled circle, on the image. */       
void gari_image_blenddisk(GariImage * image, int x, int y, int r, GariColor color);

/** Draws a blended hoop, which is a circle outline, on the image. */       
void gari_image_blendhoop(GariImage * image, int x, int y, int r, GariColor color);

/** Draws a blended floodfill. */
void gari_image_blendflood(GariImage * image, int x, int y, GariColor color);


/* Text drawing functions. Only support UTF-8. */
int gari_image_text(GariImage * dst, int x, int y,  
                    char * utf8, GariColor color);

/* Fast blit from image to image. */
void gari_image_blit(GariImage * dst, int dstx, int dsty, GariImage * src);

/** Blits a part of one image to another one with the given coordinates. Takes the indicated parts of the image, as specified by srcx, srcy, srcw and srch  */
void gari_image_blitpart(GariImage * dst, int dstx, int dsty, GariImage * src,
int srcx, int srcy, int srcw, int srch);

/* Copy part from image to image. */
int gari_image_copy(GariImage * dst, int dstx, int dsty, GariImage * src,
int srcx, int srcy, int w, int h);

/* Copy pixel from image to image. */
void gari_image_copypixel(GariImage * dst, int dstx, int dsty, 
                          GariImage * src, int srcx, int srcy);

/** Blits (a part of) an image to another one, 
    scaling it arbitrarily to the wanted size. Uses no 
    interpolation nor smoothing. 
*/
void gari_image_scaleblit(GariImage * dst, int dstx, int dsty, 
                                      int dsth, int dstw, 
                                      GariImage * src,
                                      int srcx, int srcy, 
                                      int srch, int srcw);

/** Flood fills (a part of) the image with color starting from x and y. */
void gari_image_flood(GariImage * image, int x, int y, GariColor color);


/* Input event handling. */


#define GARI_EVENT_NONE           0
#define GARI_EVENT_ACTIVE         1
#define GARI_EVENT_KEYDOWN        2
#define GARI_EVENT_KEYUP          3
#define GARI_EVENT_MOUSEPRESS     4
#define GARI_EVENT_MOUSERELEASE   5
#define GARI_EVENT_MOUSEMOVE      6
#define GARI_EVENT_MOUSESCROLL    7
#define GARI_EVENT_JOYMOVE        8
#define GARI_EVENT_JOYPRESS       9
#define GARI_EVENT_JOYRELEASE    10
#define GARI_EVENT_JOYHAT        11
#define GARI_EVENT_JOYBALL       12
#define GARI_EVENT_RESIZE        13
#define GARI_EVENT_EXPOSE        14
#define GARI_EVENT_QUIT          15
#define GARI_EVENT_USER          16
#define GARI_EVENT_SYSTEM        17
#define GARI_EVENT_LAST          18
#define GARI_JOYHAT_CENTER       0x00 
#define GARI_JOYHAT_UP           0x01
#define GARY_JOYHAT_RIGHT        0x02
#define GARY_JOYHAT_DOWN         0x04
#define GARY_JOYHAT_LEFT         0x08


/** The event structure. It's not a union for ease of wrapping in other 
languages, though only some of the fields may contain useful data depending 
on the event.kind field. */
struct GariEvent_ {
  uint8_t   kind;
  uint8_t   gain;
  uint8_t   which;
  uint8_t   sub;
  uint16_t  key;
  uint16_t  mod;
  uint16_t  unicode;
  uint16_t  x, y, xrel, yrel, w, h;
  uint16_t  button;
  int16_t   value;
};

struct GariEvent_;
typedef struct GariEvent_ GariEvent;


/** Gets an event from the event queue. Returns a special event with 
event.kind == GARI_EVENT_NONE if no events are on the queue. 
*/
GariEvent gari_event_poll();

/** Polls the event queue and copies it to the given event  addres, 
* wich must be a valid address and not be null.
* Return NULL if the eevnt queue was ampty, non-null if there was an event fetched. 
*/
GariEvent * gari_event_fetch(GariEvent * event);

/** Polls the event queue and returns a newly alloctaded GariEvent with 
* the top event of the queue. Returns NULL, and allocates no memory if 
* the queue is empty.
*/
GariEvent * gari_event_pollnew();

/** Frees memory associated with a Garievent alloctated by gari_event_pollnew.*/
void gari_event_free(GariEvent * event);


/* Fonts and text rendering. */

struct GariFont_;
/** Opaque struct for font handling. */
typedef struct GariFont_ GariFont;

/** Ways to render the font.  Default is GariFontSolid */
enum GariFontMode_ {
  GariFontSolid,
  GariFontShaded,
  GariFontBlended,
};

typedef enum GariFontMode_ GariFontMode;

/** Font styles and effects like, bold, italic, underline. */
enum GariFontStyle_ {
  GariFontNormal    = 0,
  GariFontItalic    = 1,
  GariFontBold      = 2, 
  GariFontUnderline = 4,
};

typedef enum GariFontStyle_ GariFontStyle;


/** Loads one of the fonts in a font file. */
GariFont * gari_font_loadindex(char * filename, int ptsize, long index);
/** Loads the first font in a font file. */
GariFont * gari_font_load(char * filename, int ptsize);

/** Sets the drawing mode of the font. */
GariFont * gari_font_mode_(GariFont * font, int mode);

/** Gets the drawing mode of the font. */
int gari_font_mode(GariFont * font);

/** Frees the memory allocated by the font. */
void gari_font_free(GariFont *font); 

/** Renders the font to a surface, depending on the font's settings. */
GariImage * gari_font_render(GariFont * font, char * utf8, 
    GariColor fgrgba, GariColor bgrgba);

/** Draws font with given colors. */
void gari_font_drawcolor(GariImage * image, int x, int y, char * utf8, 
                         GariFont  * font , GariColor fg, GariColor bg); 

/** Draws font with given color components. */
void gari_font_draw(GariImage * image, int x, int y, char * utf8, GariFont * font, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, uint8_t bg_r, uint8_t bg_b, uint8_t bg_a);

/** Draws font in printf style. Won't work on platforms that lack vsnprintf.
* Will draw up to 2000 bytes of characters.  
*/
void gari_font_printf(GariImage * image, int x, int y, GariFont * font, GariColor fg, GariColor bg, char * format, ...);

/** Returns a text with details about the last error in loading or 
handling a font. */
char * gari_font_error();

/** Returns the width that the given UTF-8 encoded text would be if it was rendered using gari_fonr_render. */
int gari_font_renderwidth(GariFont * font, char * utf8); 

/** Returns the font's max height */
int gari_font_height(GariFont * font); 

/** Returns the font's font max ascent (y above origin)*/
int gari_font_ascent(GariFont * font); 

/** Returns the font's min descent (y below origin)*/
int gari_font_descent(GariFont * font);

/** Returns the font's recommended line spacing. */
int gari_font_lineskip(GariFont * font);

int gari_fontstyle_tottf(int style);

int gari_fontstyle_fromttf(int style); 

/** Returns the style of the font.  */
int gari_font_style(GariFont * font);

/** Sets the style of the font. Note that the style may be "faked" 
by the underlaying implementation. Use a suitably pre-modified font for 
better effects. */
void gari_font_style_(GariFont * font, int style); 

/** Loads font from a data buffer in memory */
GariFont * gari_font_dataindex(const unsigned char * data, int datasize, 
                               int ptsize, long index);

/** Loads font from a data buffer in memory */
GariFont * gari_font_data(const unsigned char * data, int datasize, int ptsize);

/** Built in public domain font data and size (tuffy) by Ulrich Tatcher. */
extern const int              data_font_tuffy_ttf_size;
extern const unsigned char *  data_font_tuffy_ttf;

/** Music and sound. */

#define GARI_AUDIO_LOWFREQENCY 8000
#define GARI_AUDIO_MEDIUMFREQENCY 22050
#define GARI_AUDIO_HIGHFREQENCY 44100

struct GariAudioInfo_;
typedef struct GariAudioInfo_ GariAudioInfo;

/** Allocates a new audioinfo. */
GariAudioInfo * gari_audioinfo_new(int freq, int form, int chan); 

/** Frees an allocated audioinfo. */
GariAudioInfo * gari_audioinfo_free(GariAudioInfo * info); 

/** Gets the frequency of an audio info  */
int gari_audioinfo_frequency(GariAudioInfo * info);

/** Gets the amount of channels of an audio info  */
int gari_audioinfo_channels(GariAudioInfo * info); 

/** Gets the format of an audio info  */
int gari_audioinfo_format(GariAudioInfo * info); 

/** Queries the current state of the audio system. 
Returns NULL if it was not initialized yet. */
GariAudioInfo * gari_audio_query();

/** Initializes the audio subsystem for a game. 
Free it with gari_audioinfo_free */
GariGame * gari_audio_init(GariGame * game, int frequency);

/** Cleans up the audio subsystem for a game. */
void gari_audio_done(GariGame * game);

struct GariSound_;
typedef struct GariSound_ GariSound;

/** Initialises and loads sound into an existing GariSound record. */
GariSound * gari_sound_init(GariSound * sound, char * filename);

/** Creates a new GariSound and loads the sound from a file. */
GariSound * gari_sound_load(char * filename);

/** Deallocates the loaded sound, but not the wrapper GariSound itself. */
GariSound * gari_sound_done(GariSound * sound);

/** Calls gari_sound_done and then frees the sound itself.*/
GariSound * gari_sound_free(GariSound * sound);

/** Plays a sound once. */
GariSound * gari_sound_play(GariSound * sound);


struct GariMusic_;
typedef struct GariMusic_ GariMusic;

/** Initialises and loads music into an existing GariMusic record. */
GariMusic * gari_music_init(GariMusic * music, char * filename);

/** Creates a new GariMusic and loads the music from a file. */
GariMusic * gari_music_load(char * filename);

/** Deallocates up the loaded music, but not the wrapper GariMusic itself. */
GariMusic * gari_music_done(GariMusic * music);

/** Calls gari_music_done and then frees the music itself.*/
GariMusic * gari_music_free(GariMusic * music);

/** Starts playing the music loop times (-1 means to keep on repeating) 
*   fading in after fade ms. 
*/
GariMusic * gari_music_fadein(GariMusic * music, int loops, int fade); 

/** Stops playing the music, fading out after fade ms. */
GariMusic * gari_music_fadeout(GariMusic * music, int fade); 


 


/* Higher level functions and structs. */

struct GariStyle_;
typedef struct GariStyle_ GariStyle;

GariStyle * gari_style_free(GariStyle * style); 

GariStyle * gari_style_new(GariColor   fore, 
                           GariColor   back,
                           GariFont  * font,
                           GariImage * image
                          );

GariFont * gari_style_font(GariStyle * style); 

GariColor gari_style_fore(GariStyle * style);
 
GariColor gari_style_back(GariStyle * style);

GariImage * gari_style_image(GariStyle * style); 




/** Camera  A camera models a 2D point of view over a tile map, etc. */
struct GariCamera_;
typedef struct GariCamera_ GariCamera;


/* Known particle engine effects: */
enum GariFlowKind_ {
  GariFlowSnow,
  GariFlowRain,
  GariFlowBlood,
  GariFlowSlash,
  GariFlowExplode,
  GariFlowDamage,
};
typedef enum GariFlowKind_ GariFlowKind;


/** A drop is a particle in the particle engine */
struct GariDrop_;
typedef struct GariDrop_ GariDrop;

/** A well is an initial configuration af a certain amount of GariDrop 
  particles in a GariFlow particle engine. */
struct GariWell_;
typedef struct GariWell_ GariWell;

/** A Flow is an instance of a particle engine. 
*/
struct GariFlow_;
typedef struct GariFlow_ GariFlow;

/** Particle engine init callback function. */
typedef GariDrop * GariDropInitFunction(GariDrop * data, GariWell * well); 

/** Particle engine draw callback function. */
typedef GariDrop * GariDropDrawFunction(GariDrop * data, GariImage * im); 

/** Particle engine update callback function. */
typedef GariDrop * GariDropUpdateFunction(GariDrop * data, int time); 

/** Allocates a new GariFlow particle engine with the given amount of 
*   particles available. Free it after use with gari_flow_free(); 
*/
GariFlow * gari_flow_make(size_t size);

/**
* Free and destroy a previously allocated GariFlow particle engine.
*/
void gari_flow_free(GariFlow * flow); 

/**
* Updates the position of the pixels after time ticks passed.
*/
void gari_flow_update(GariFlow * flow, int time); 
 
/** Draws all active particles from tthis flow to the screen. */
void gari_flow_draw(GariFlow * flow, GariImage * im); 

/**
* Activates amount Drops (particles) in the GariFlow flow. 
* The particle will be of kind kind, and will be placed, depending on the 
* kind, at x a,d y, and displayed using given color, image or text 
*/
GariFlow * gari_flow_activate(GariFlow * flow, 
      int amount, int kind, int x, int y, GariColor color, 
      GariImage * im, char * text);

/** Initializes the random number generator, 
    used mainly in the particle engine. */
void gari_random_init(); 

/** Gets a random number between min and max. */
long gari_random(long min, long max);




/** A sprite is a fully aniated object that can be animated in different ways * depending on it's facing direction and actions.
* For example, the sprite may have a walking action in 2 or 4 directions,
* which each have a different animation.
* A sprite may also be consisted of different parts or layers superimposed 
* over each other, for example, a character holding a weapon, etc.  
*/
struct GariSprite_;
typedef struct GariSprite_ GariSprite;

/** An animation is a full cycle of an animation, 
*   of which sprites are made up. 
*/
struct GariAnime_;
typedef struct GariAnime_ GariAnime;

/** Anime can be described as a crude "program", 
  using the following constants: */
enum GariAnimeProgram_ {
  GariAnimeRewind,
  GariAnimeNext,
  GariAnimeWait
};


/** 
 * A tile is a set of images that gets animated according to a 
 * fixed pattern, and displayed as a single part of a layer in a map. 
 */
struct GariTile_; 
typedef struct GariTile_ GariTile;

/** Flags for tiles that deterine their in-game properties. */
enum GariTileFlags_ {   
    GariTileWalk = 1 << 0, // The tile can be walked over.
    GariTileJump = 1 << 1, // The tile can be jumped over.
    GariTileLedge= 1 << 2, // The tile will cause a jump when walked over.
    GariTileSwim = 1 << 3, // The tile is some kind of swimmable liquid.
    GariTilePain = 1 << 4, // Walking over the tile causes pain.
    GariTileUp   = 1 << 5, // The tile is a step-up stair to the next layer.
    GariTileDown = 1 << 6, // The tile is a step-down to the previous layer.    
    GariTileEdge = 1 << 7, // The tile blocks anything from being pushed inside
    GariTileSlip = 1 << 8, // The tile is slippery, like ice, etc.    
    GariTilePush = 1 << 9, // The tile will "push" the player, 
                           // like a conveyor belt, etc.
    // 1<<10, 1<<11 are reserved
    GariTileNorth= 1 <<12, // The direction of this tile's effect.
    GariTileEast = 1 <<13, // The direction of this tile's effect.
    GariTileSouth= 1 <<14, // The direction of this tile's effect.
    GariTileWest = 1 <<15, // The direction of this tile's effect.
};

/** Tile kinds that also help determine properties. */
enum GariTileKinds_ {
  GariTileFloor ,
  GariTileWall  ,
  GariTileWater ,
};

/** A tileset is a set of tiles that are used by a tile map. */
struct GariTileset_; 
typedef struct GariTileset_ GariTileset;

/** 
* A layer is a single layer in a 2D game tile map.
* A layer can consists of a background and/or tiles.
*/
struct GariLayer_; 
typedef struct GariLayer_ GariLayer;

/** 
* A tile map consists of individual tiles. Tiles are simply indirect 
* pointers to images, where the pointer can be changed to animate it. 
* The same idea is used for sprites, and unified in the GariSheet struct.
*/
struct GariSheet_;
typedef struct GariSheet_ GariSheet;

/** Sets the active image of the GariSheet. */
GariSheet * gari_sheet_image_(GariSheet * sheet, GariImage * image);
 
/** Gets the active image of the GariSheet. */
GariImage * gari_sheet_image(GariSheet * sheet); 

/** Allocates a new GariSheet that has imageas it's active image.  */
GariSheet * gari_sheet_new(GariImage * image); 

/** Frees the memory associated with a previously allocated GariSheet. */
GariSheet * gari_sheet_free(GariSheet * sheet); 

/** Draws a GariSheet to an image. */
void gari_image_blitsheet(GariImage * dst, int x, int y, GariSheet * sheet);

/** Initializes a GariLayer. Gridwide and gridhigh are the size of 
the tile map's grid. tilewide and tilehigh are the sizes of the tiles used. */
GariLayer * gari_layer_init(GariLayer * layer,  
                           int gridwide, int gridhigh, 
                           int tilewide, int tilehigh); 

/** Allocates a new gari layer with the given parameters. */
GariLayer * gari_layer_new(int gridwide, int gridhigh, 
                           int tilewide, int tilehigh);
                            
/** Deallocates memory allocated by gai_layer_new */ 
GariLayer * gari_layer_free(GariLayer * layer); 

/** Returns TRUE if the given gridx and gridy are outside the grid
 Returns FALSE if inside the grid.
*/
int gari_layer_outsidegrid(GariLayer * layer, int gridx, int gridy); 

/** Sets the tile at the given location to the given GariSheet pointer, 
* which may be NULL. Returns the layer, or NULL on error.  
*/
GariLayer * gari_layer_set(GariLayer * layer, 
                           int gridx, int gridy, GariSheet * tile);

/** Returns the sheet in the layer's grid at the given grid coordinates,
* returns NULL if the fcoordinates are out of bounds or if it was an empty tile.
*/
GariSheet * gari_layer_get(GariLayer * layer, int gridx, int gridy); 

/** Draws the tile layer, with x and y as the top left corner, 
* to the given image. X and y may be negative. 
*/
void gari_layer_draw(GariLayer * layer, GariImage * image, int x, int y); 

/** Returns the width of the layer in grid units. Returns -1 on error. */
int gari_layer_gridwide(GariLayer * layer); 

/** Returns the height of the layer in grid units. Returns -1 on error. */
int gari_layer_gridhigh(GariLayer * layer);

/** Returns the width of the layer's tiles in pixels. Returns -1 on error. */
int gari_layer_tilewide(GariLayer * layer);

/** Returns the height of the layer's tiles in pixels. Returns -1 on error. */
int gari_layer_tilehigh(GariLayer * layer);

/** Returns the width of the layer in pixels. Returns -1 on error. */
int gari_layer_wide(GariLayer * layer);

/** Returns the height of the layer in pixels. Returns -1 on error. */
int gari_layer_high(GariLayer * layer);


/**
* A map is the visual representation of the game's playfield, which 
* consists of several layers and may contain several sprites.  
*/
struct GariMap_; 
typedef struct GariMap_ GariMap;


/** Joystick handling. */
struct GariJoystick_ ;
typedef struct GariJoystick_ GariJoystick; 

/** Returns the amount of joysticks connected to this system. */
int gari_joy_amount();

/** Returns the name of the inde'th joystick or NULL if no such joystick. */
const char * gari_joy_nameindex(int index); 

/** Returns the name of a joystick object. */
const char * gari_joy_name(GariJoystick * joy);
 
/** Returns the index of a joystick object. */
int gari_joy_index(GariJoystick * joy);

/** Returns the amount of axes of a joystick object. */
int gari_joy_axes(GariJoystick * joy); 

/** Returns the amount of buttons of a joystick object. */
int gari_joy_buttons(GariJoystick * joy);

/** Returns the amount of buttons of a joystick object. */
int gari_joy_balls(GariJoystick * joy); 

/** Describes a joystick to stderr. */
void gari_joy_describe(GariJoystick * joy); 

/** Opens a joystick. Memory is allocated, so call gari_joy_free when done. */
GariJoystick * gari_joy_open(int index); 

/** Closes the joystick and frees associated memory. */
void gari_joy_free(GariJoystick * stick); 

/** Returns how many joysticks are available. */
int gari_game_joysticks(GariGame * game); 

/** Returns the n-th joystick of the game. */
GariJoystick * gari_game_joystick(GariGame * game, int index);


/** Time specific funtions.  */
/** Milliseconds since gari_game_make was called */
uint32_t gari_game_ticks(GariGame * game);

/** 2d vectors, for speedy vector math.  */

typedef float GariFloat; 

/** A two-dimensional vector. */
struct GariVector_;
typedef struct GariVector_ GariVector;

struct GariVector_     {
  GariFloat x, y;
};

typedef const GariVector GariCVector;

/** Makes a new GariVector. */
GariVector gari_vector(GariFloat x, GariFloat y); 

/** Initializes a GariVector */
GariVector * gari_vector_init(GariVector * vec, GariFloat x, GariFloat y);

/** Allocates and initializes a GariVector and returns it. Retuns NULL if out 
   of memory. */
GariVector * gari_vector_new(GariFloat x, GariFloat y);

/** Allocates and initializes a Garivectory and returns it. Retuns NULL if out 
   of memory. */
GariVector * gari_vector_newvector(GariVector v1);

/** Frees a GariVector's allocated memory. Returns NULL. */
GariVector * gari_vector_free(GariVector * vec);

/** Checks if a vector is zero. Returns nonzero if it is, zero if not. */
int gari_vector_zero_p(const GariVector v1);

/** Returns a zero vector. */
GariVector gari_vector_zero();

/** Compares two vectors and sees if they are within delta of each other. */
int gari_vector_equal_delta(const GariVector v1, const GariVector v2, 
                            GariFloat delta);
                            
/** Compares two vectors for strict equality. */
int gari_vector_equal(const GariVector v1, const GariVector v2);

/** Vector addition. */
GariVector gari_vector_add(GariVector v1, GariVector v2); 

/** Vector substraction. */
GariVector gari_vector_sub(GariVector v1, GariVector v2); 

/** Vector negation. */
GariVector gari_vector_neg(GariVector v1); 

/** Scalar Vector multiplication. */
GariVector gari_vector_mul(GariVector v1, GariFloat f); 

/** Scalar Vector division. */
GariVector gari_vector_div(GariVector v1, GariFloat f); 

/** Vector dot product. */
GariFloat gari_vector_dot(GariVector v1, GariVector v2); 

/** 
Cross product magnitude. The cross product of 2D x,y vectors is 
a 3D vector with a z component, so this function returns only the magnitude of
that z component.  
*/
GariFloat gari_vector_cross(GariVector v1, GariVector v2);

/** Perpendicular vector, rotated by 90 degrees, anticlockwise. */
GariVector gari_vector_perp(GariVector v1);

/** Perpendicular vector, rotated by -90 degrees, clockwise. */
GariVector gari_vector_nperp(GariVector v1);

/** Returns the dot poduct of the vector and itself. */
GariFloat gari_vector_dotself(GariVector v1);

/** Returns the squared length of the vector. Same as dotself, really. 
Useful for comparing lengths when speed is of importance.  */
GariFloat gari_vector_lensq(GariVector v1);

/** Returns the length of the vector. */
GariFloat gari_vector_len(GariVector v1);

/** Returns a vector that is the projection of v1 onto v2*/
GariVector gari_vector_project(GariVector v1, GariVector v2);

/** Rotates v1 by v2. */
GariVector gari_vector_rotate(GariVector v1, GariVector v2); 
 
/** Inverse rotation. */
GariVector gari_vector_unrotate(GariVector v1, GariVector v2);

/** Linear interpolation on a line between between v1 and v2. Returns a vector 
that points to a point on the line between v1 and v2. */
GariVector gari_vector_lerp(GariVector v1, GariVector v2, GariFloat tx);

/** Returns v1 normalized. Not safe for division by 0 in case length is 0. */
GariVector gari_vector_normalize_unsafe(GariVector v1); 

/** Returns v1 normalized. Safe for division by 0 in case length is 0. */
GariVector gari_vector_normalize(GariVector v1);

/** If the length of the vector is greater than max, this function 
returns a vector with the same direction as v1, but with the length 
of that vector limited to max. Otherwise, it returns v1  */
GariVector gari_vector_max(GariVector v1, GariFloat max); 

/** Returns the distance between v1 and v2. */
GariFloat gari_vector_dist(GariVector v1, GariVector v2);
 
/** Returns the squared distance between v1 and v2. */
GariFloat gari_vector_distsq(GariVector v1, GariVector v2); 

/** Returns true if the distance between v1 and v2 is less than d. */
int gari_vector_near(GariVector v1, GariVector v2, GariFloat d);

/** Retuns a unit vector that makes an angle a with the X axis. */
GariVector gari_vector_fromangle(GariFloat f);
 
/** Returns the angle of the vector with the X axis in clockwise 
direction in radians */
GariFloat gari_vector_angle(GariVector v1);
 





#endif
