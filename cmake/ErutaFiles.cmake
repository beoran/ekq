# All source files except main.c to enable testing
# by using cmake's fake library feature. Don't forget to add new source 
# files here manually.
set(BF_SRC_FILES
  src/bf/BFObject.c
  src/bf/BFClass.c
  src/bf/BFArray.c
  src/bf/BFMap.c
  src/bf/BFInteger.c
  src/bf/BFDouble.c
  src/bf/BFString.c
  src/bf/BFValue.c
)

set(ERUTA_SRC_FILES
  src/alps.c
  src/area.c
  src/bad.c
  src/bevec.c
  src/brex.c
  src/bump.c
  src/bumpshape.c  
  src/bxml.c
  src/bxmlparser.c
  src/camera.c
  src/callrb.c
  src/draw.c
  src/dynar.c
  src/effect.c
  src/event.c
  src/every.c
  src/flags.c
  src/fifi.c
  src/glh.c
  src/hatab.c
  src/ifa.c
  src/inli.c
  src/intgrid.c
  src/laytext.c
  src/mem.c
  src/mobile.c
  src/mode.c
  src/monolog.c
  src/obj.c
  src/pique.c
  src/pointergrid.c
  src/react.c
  src/rebl.c
  src/rebox.c
  src/resor.c
  src/rh.c    
  src/scegra.c
  src/ses.c
  src/silut.c
  src/sound.c
  src/sprite.c
  src/spritelist.c
  src/spritestate.c
  src/spritelayout.c
  src/state.c
  src/store.c
  src/str.c
  src/thing.c
  src/tile.c
  src/tileio.c
  src/tilemap.c
  src/tilepane.c
  src/tarray.c
  src/tmatrix.c
  src/toruby.c
  src/tr_audio.c
  src/tr_path.c
  src/tr_store.c
  src/tr_graph.c
  src/tr_sprite.c
  src/tr_thing.c
  src/ui.c
  src/utf8.c
  src/widget.c
  src/xresor.c
  src/zori.c
)
