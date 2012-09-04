#ifndef MAIN_PROTO_H
#define MAIN_PROTO_H
/*
This file was autogenerated from src/main.c
by bin/genproto
Please do not hand edit.
*/

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
void puts_standard_path(int path, char * name);

    /** Initialises the reactor, the game state is it's data. */
    react_initempty(&react, state);
    react.keyboard_key_up   = main_react_key_up;
    react.keyboard_key_down = main_react_key_down;
    
    puts_standard_path(ALLEGRO_EXENAME_PATH, "ALLEGRO_EXENAME_PATH:");

    camera  = state_camera(state);
    //music   = music_load("musictest.ogg");
    // if(!music) perror("musictest.ogg");

    border  = fifi_loadbitmap("border_004.png",
                            "image", "ui", "background", NULL);

    border  = fifi_loadbitmap_vpath("image/ui/background/border_004.png");
    sheet   = fifi_loadbitmap("tiles_village_1000.png", "image", "tile", NULL);
    // image_load(ERUTA_TEST_SHEET);
    if(!sheet);

#endif // MAIN_PROTO_H

