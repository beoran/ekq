#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>


struct Stamod {
  ALLEGRO_VERTEX * vertices;
  size_t vspace;
  size_t vsize;
  
  ALLEGRO_BITMAP * texture;
  
  ALLEGRO_VERTEX * triangles;
  size_t tspace;
  size_t tsize;
};

#define STAMOD_INIT_AMOUNT 1024

struct Stamod * stamod_done(struct Stamod * me) {
  if (!me) return NULL;
  
  if (me->triangles) {
    free(me->triangles);
    me->triangles = NULL;
  }
  
  if (me->vertices) {
    free(me->vertices);
    me->vertices = NULL;
  }
  
  me->vspace = me->vsize = 0;
  me->tspace = me->tsize = 0;
  
  if (me->texture) {
    al_destroy_bitmap(me->texture);
    me->texture = NULL;
  }
  
  return NULL;
}

struct Stamod * stamod_init(struct Stamod * me, ALLEGRO_BITMAP * texture) {
  if (!me) return NULL;
  me->triangles = NULL;
  me->texture   = texture;
  me->vertices  = calloc(STAMOD_INIT_AMOUNT,  sizeof(ALLEGRO_VERTEX));
  if (!me->vertices) return stamod_done(me);
  me->vspace = STAMOD_INIT_AMOUNT;
  me->vsize  = 0;
  
  me->triangles  = calloc(STAMOD_INIT_AMOUNT * 3,  sizeof(ALLEGRO_VERTEX));
  if (!me->triangles) return stamod_done(me);
  me->tspace = STAMOD_INIT_AMOUNT;
  me->tsize  = 0;
   
  return me;
}


int stamod_add_vertex(struct Stamod * me, ALLEGRO_VERTEX * v) {   
  int result;
  if (!me) return -1;
  if (!me->vertices) return -2; 
  if (!v) return -3;
  if (me->vsize <= (me->vspace - 1)) {
    ALLEGRO_VERTEX * aid = realloc(me->vertices, (me->vspace + STAMOD_INIT_AMOUNT) * sizeof(ALLEGRO_VERTEX));
    if (!aid) return -4;
    me->vspace += STAMOD_INIT_AMOUNT;
    me->vertices = aid; 
  } 
  me->vertices[me->vsize] = (*v);
  result = me->vsize++;
  return result;
}

int stamod_add_triangle(struct Stamod * me, ALLEGRO_VERTEX *v[3]) {   
  int result;
  if (!me) return -1;
  if (!me->triangles) return -2; 
  if (me->tsize <= (me->tspace - 1)) {
    ALLEGRO_VERTEX * aid = realloc(me->triangles, (me->tspace + STAMOD_INIT_AMOUNT) * sizeof(ALLEGRO_VERTEX) * 3);
    if (!aid) return -4;
    me->tspace += STAMOD_INIT_AMOUNT;
    me->triangles = aid; 
  } 
  me->triangles[me->tsize * 3    ] = *(v[0]);
  me->triangles[me->tsize * 3 + 1] = *(v[1]);
  me->triangles[me->tsize * 3 + 2] = *(v[2]);
  result = me->tsize++;
  return result;
}

void init_vertex(ALLEGRO_VERTEX * vertex, float x, float y, float z, float u, float v, float r, float g, float b, float a) {
  vertex->x = x;
  vertex->y = y; 
  vertex->z = z;
  vertex->u = u;
  vertex->v = v;
  vertex->color = al_map_rgba_f(r, g , b, a);
}


int stamod_new_vertex(struct Stamod * me, float x, float y, float z, 
                      float u, float v, float r, float g, float b, float a) {
    ALLEGRO_VERTEX vert;
    init_vertex(&vert, x, y, z, u, v, r, g, b, a);
    return stamod_add_vertex(me, &vert);
}


ALLEGRO_VERTEX * stamod_get_vertex(struct Stamod * me, int index) {
  if (!me) return NULL;
  if (!me->vertices) return NULL; 
  if (index < 0) return NULL;
  if (me->vsize <= (index)) return NULL;
  return me->vertices + index;
}

int stamod_new_tri(struct Stamod * me, int i1, int i2, int i3) {
    ALLEGRO_VERTEX  *aid[3];
    aid[0] = stamod_get_vertex(me, i1);
    aid[1] = stamod_get_vertex(me, i2);
    aid[2] = stamod_get_vertex(me, i3);
    if (!aid[0]) return -1;
    if (!aid[1]) return -2;
    if (!aid[2]) return -3;
    return stamod_add_triangle(me, aid);
}

 


void init_vertex_color(ALLEGRO_VERTEX * vertex, float x, float y, float z, float u, float v, ALLEGRO_COLOR color) {
  vertex->x = x;
  vertex->y = y; 
  vertex->z = z;
  vertex->u = u;
  vertex->v = v;
  vertex->color = color;
}

void stamod_draw(struct Stamod * me) {     
  int stop = me->tsize * 3;
  al_draw_prim(me->triangles, NULL, me->texture, 0, stop, ALLEGRO_PRIM_TRIANGLE_LIST);
}


void draw_textured_colored_rectangle_3d(
  float x, float y, float z, float w, float h, float d, float up, float vp,
  ALLEGRO_BITMAP * texture,  ALLEGRO_COLOR color) {
    float u, v;
    float tw, th;
    ALLEGRO_VERTEX p[4];
    if (texture) {
      tw = al_get_bitmap_width(texture);
      th = al_get_bitmap_height(texture);
      u  = tw * up;
      v  = th * vp;
    } else {
      u = v = 0.0f;
    }
    
    init_vertex_color(p+0, x    , y     ,  z    ,   0,   0, color);
    init_vertex_color(p+1, x    , y + h ,  z + d,   0,   v, color);
    init_vertex_color(p+2, x + w, y     ,  z + d,   u,   0, color);
    init_vertex_color(p+3, x + w, y + h ,  z + d,   u,   v, color);
    al_draw_prim(p, NULL, texture, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
}


int main(void) {
  ALLEGRO_VERTEX        v[8];
  ALLEGRO_DISPLAY     * display;
  ALLEGRO_TRANSFORM     perst;
  ALLEGRO_TRANSFORM     camt;
  ALLEGRO_BITMAP      * texture, * texture2;
  ALLEGRO_EVENT_QUEUE * queue;
  ALLEGRO_EVENT         event;
  ALLEGRO_FONT        * font;
  int                   busy  = 1;
  float                 cx    = 0;
  float                 cy    = -2; // 128;
  float                 cz    = 0;
  int                   face  = 0; 
  int                   hori  = 0;
  float                 angle = 0;
  float                 theta = 0;  
  float                 near  = 2;
  float                 far   = 8192;
  float                 zoom  = 1;
  float                 scale = 1.0;
  float                 us    = 20; 
  float                 ratio = 480.0 / 640.0;
  al_init();  
  al_init_image_addon();
  al_init_font_addon();
  al_install_keyboard();
  font  = al_create_builtin_font();
  
  queue = al_create_event_queue();  
  al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
  
  display  = al_create_display(640, 480);
  al_register_event_source(queue, al_get_keyboard_event_source());  
  texture  = al_load_bitmap("tile_1.png");
  texture2 = al_load_bitmap("tile_2.png");
  
  /* Allegro coordinates: +Y is down, +X is to the right, 
   * and +Z comes "out of" the screen.
   * 
   */
  
  
  while (busy) {
    init_vertex(v+0,   0,   0,   0,   0,   0, 1, 0, 0, 1);
    init_vertex(v+1,   0,   0,  us,   0, 256, 0, 1, 0, 1);
    init_vertex(v+2,  us,   0,  us, 256, 256, 0, 0, 1, 1);
    init_vertex(v+3,  us,   0,   0, 256,   0, 1, 1, 0, 1);
    init_vertex(v+4,  us, -us,   0, 256, 256, 1, 0, 1, 1);
    init_vertex(v+5,   0, -us,   0, 256,   0, 0, 1, 1, 1); 
    init_vertex(v+6,   0, -us,  us, 256, 256, 0, 0, 0, 1); 
    init_vertex(v+7,   0,   0,  us,   0, 256, 1, 1, 1, 1); 
  
  
    al_identity_transform(&camt);
    al_scale_transform_3d(&camt, scale, scale, scale);
    al_translate_transform_3d(&camt, cx, cy, cz);
  
    angle = face * 0.125 * ALLEGRO_PI;
    al_rotate_transform_3d(&camt, 0, -1, 0, angle); 
    theta = hori * 0.125 * ALLEGRO_PI;
    al_rotate_transform_3d(&camt, -1, 0, 0, theta); 
    
    al_use_transform(&camt);
    // al_set_projection_transform(display, &perst);

  
    al_clear_to_color(al_map_rgb_f(0.75, 0.75, 0.95));
    al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
    al_clear_depth_buffer(far);

    
    
    al_identity_transform(&perst);
    al_perspective_transform(&perst, -1, ratio, near, 
                                      1,  -ratio, far);
    al_use_projection_transform(&perst);

    al_draw_filled_rectangle(0, 0, 4, 5, al_map_rgb_f(0, 0.25, 0.25));
    
        
    al_draw_prim(v, NULL, texture, 0, 8, ALLEGRO_PRIM_TRIANGLE_FAN);
    
    draw_textured_colored_rectangle_3d(0 , -us, 0, us, us, 0, 
                                       1.0, 1.0, texture2, al_map_rgb_f(1,1,1));
    
    draw_textured_colored_rectangle_3d(0 , -us, 0, 0, us, us, 
                                       1.0, 1.0, texture2, al_map_rgb_f(1,1,1));

    draw_textured_colored_rectangle_3d(0 , 0, 0, us, 0, us,
                                       1.0, 1.0, texture2, al_map_rgb_f(1,1,1));
     
    
    al_identity_transform(&perst);
    al_orthographic_transform(&perst, 0, 0, -1.0, 640, 480, 1.0);
 
    al_identity_transform(&camt);
    al_use_projection_transform(&perst);
    al_use_transform(&camt);
    al_draw_filled_rectangle(111, 22, 133, 44, al_map_rgb_f(0.25, 0.25, 0));

    
    al_draw_multiline_textf(font, al_map_rgb_f(1,1,1), 10, 10, 620, 0, 0,
      "Coords: (%f %f %f)\nAngle: (%f %f)\nView: [%f %f %f %f]\nScale: %f", 
               cx, cy, cz,        angle, theta,   near, far, zoom, scale); 
    
    
    
    al_flip_display();  
    al_wait_for_event(queue, &event);
    
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) { 
      switch (event.keyboard.keycode) {
        case ALLEGRO_KEY_RIGHT:
          cx += 8;
          break;

        case ALLEGRO_KEY_LEFT:
          cx -= 8;
          break;
          
        case ALLEGRO_KEY_UP:
          cy += 8;
          break;
          
        case ALLEGRO_KEY_DOWN:
          cy -= 8;
          break;  
        
        case ALLEGRO_KEY_HOME:
          cz += 8;
          break;
          
        case ALLEGRO_KEY_END:
          cz -= 8;
          break;  
          
        case ALLEGRO_KEY_R:
          face++;
          break;
      
        case ALLEGRO_KEY_L:
          face--;
          break;
          
        case ALLEGRO_KEY_H:
          hori++;
          break;
      
        case ALLEGRO_KEY_G:
          hori--;
          break;
  
          
        case ALLEGRO_KEY_N:
          near *= 2.0;
          break;
          
        case ALLEGRO_KEY_M:
          near /= 2.0;
          break;

        case ALLEGRO_KEY_F:
          far += 64;
          break;
          
        case ALLEGRO_KEY_V:
          far -= 64;
          break;
        
        case ALLEGRO_KEY_Z:
          zoom *= 2.0f;
          break;
          
        case ALLEGRO_KEY_S:
          zoom /= 2.0f;
          break;
        
        case ALLEGRO_KEY_A:
          scale *= 2.0f;
          break;
          
        case ALLEGRO_KEY_Q:
          scale /= 2.0f;
          break;
          
        case ALLEGRO_KEY_ESCAPE:
          busy = 0 ;
          break;
  
        default:
          break;
      }
    }
  }
  
  al_destroy_bitmap(texture);
  al_destroy_bitmap(texture2);
  
  
  return 0;  
}
