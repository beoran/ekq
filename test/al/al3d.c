#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>


void init_vertex(ALLEGRO_VERTEX * vertex, float x, float y, float z, float u, float v, float r, float g, float b, float a) {
  vertex->x = x;
  vertex->y = y; 
  vertex->z = z;
  vertex->u = u;
  vertex->v = v;
  vertex->color = al_map_rgba_f(r, g , b, a);
}

int main(void) {
  ALLEGRO_VERTEX    v[4];
  ALLEGRO_DISPLAY     * display;
  ALLEGRO_TRANSFORM     perst;
  ALLEGRO_TRANSFORM     camt;
  ALLEGRO_BITMAP      * texture;
  ALLEGRO_EVENT_QUEUE * queue;
  ALLEGRO_EVENT         event;
  ALLEGRO_FONT        * font;
  int                   busy = 1;
  float                 cx   = -100;
  float                 cy   = 100;
  float                 cz   = -100;
  int                   face = 0; 
  int                   hori = 0;
  float                 angle= 0;
  float                 theta= 0;  
  float                 near = 100;
  float                 far  = 10000;
  
  al_init();  
  al_init_image_addon();
  al_init_font_addon();
  al_install_keyboard();
  font  = al_create_builtin_font();
  
  queue = al_create_event_queue();  
  
  display = al_create_display(640, 480);
  al_register_event_source(queue, al_get_keyboard_event_source());  
  texture = al_load_bitmap("tile_1.png");
  printf("texture %p\n", texture);
  
  /* Allegro coordinates: +Y is down, +X is to the right, 
   * and +Z comes "out of" the screen.
   * 
   */
  
  while (busy) {
    init_vertex(v+0,    0,  0,   0,    0,    0, 1, 0, 0, 1);
    init_vertex(v+1,    0,  0, 200,    0,  256, 0, 1, 0, 1);
    init_vertex(v+2,  200,  0, 200,  256,  256, 0, 0, 1, 1);
    init_vertex(v+3,  200,  0,  0,  256,    0, 1, 1, 0, 1);
  
    al_identity_transform(&perst);
    al_perspective_transform(&perst, -320, -240, 64, 320, near, far);
    al_identity_transform(&camt);
    al_translate_transform_3d(&camt, cx, cy, cz);
    angle = face * 0.25 * ALLEGRO_PI;
    al_rotate_transform_3d(&camt, 0, -1, 0, angle); 
    theta = hori * 0.25 * ALLEGRO_PI;
    al_rotate_transform_3d(&camt, -1, 0, 0, angle); 

            
    al_set_projection_transform(display, &perst);
    al_use_transform(&camt);
    
    
    al_clear_to_color(al_map_rgb_f(0.75, 0.75, 0.95));
    al_draw_filled_rectangle(10, 20, 30, 40, al_map_rgb_f(0, 0.25, 0.25));
    
    al_draw_prim(v, NULL, texture, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
    
    al_identity_transform(&perst);
    al_orthographic_transform(&perst, 0, 0, -1, 640, 480, 1);
 
    al_identity_transform(&camt);
    al_set_projection_transform(display, &perst);
    al_use_transform(&camt);
    al_draw_filled_rectangle(111, 22, 133, 44, al_map_rgb_f(0.25, 0.25, 0));

    
    al_draw_multiline_textf(font, al_map_rgb_f(1,1,1), 10, 10, 620, 0, 0,
      "Coords: (%f %f %f)\nAngle: (%f %f)\nView: [%f %f]", 
               cx, cy, cz,        angle, hori,   near, far); 
    
    
    
    al_flip_display();  
    al_wait_for_event(queue, &event);
    
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) { 
      switch (event.keyboard.keycode) {
        case ALLEGRO_KEY_RIGHT:
          cx += 10;
          break;

        case ALLEGRO_KEY_LEFT:
          cx -= 10;
          break;
          
        case ALLEGRO_KEY_UP:
          cy += 10;
          break;
          
        case ALLEGRO_KEY_DOWN:
          cy -= 10;
          break;  
        
        case ALLEGRO_KEY_HOME:
          cz += 10;
          break;
          
        case ALLEGRO_KEY_END:
          cz -= 10;
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
          near += 10;
          break;
          
        case ALLEGRO_KEY_M:
          near -= 10;
          break;

        case ALLEGRO_KEY_F:
          far += 1000;
          break;
          
        case ALLEGRO_KEY_V:
          far -= 1000;
          break;
        
          
        case ALLEGRO_KEY_ESCAPE:
          busy = 0 ;
          break;
  
        default:
          break;
      }
    }
  }
  
  return 0;  
}
