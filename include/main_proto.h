#ifndef MAIN_PROTO_H
#define MAIN_PROTO_H
/*
This file was autogenerated from src/main.c
by bin/genproto on 2012-01-26 13:22:26 +0100
Please do not hand edit.
*/

/** For some reason, onlt wav music plays?
It seems there is a bug in init_dynlib() in the ogg driver nbut only
if Allegro is compiled in the default RelWithDbg mode.
***/
#define ERUTA_TEST_MUSIC "data/music/musictest.ogg"
#define ERUTA_MAP_TEST "data/map/map_0001.tmx"

int main(void);

#endif // MAIN_PROTO_H

