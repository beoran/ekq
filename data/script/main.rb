# Main eruta script

# Set this to true to start a test map in stead of the normal main menu on startup.
START_TEST_MAP = false
# Play music at startup
PLAY_MUSIC     = false
# Show main background
MAIN_BACKGROUND = true

# Helpers
script "forwardable.rb"

# Game state
script "state.rb"

# Load keycodes
script "keycode.rb"
# Load OO wrappers
script "thing.rb"
script "sprite.rb"
script "graph.rb"
script "store.rb"

# Load UI subsystem.
script 'zori.rb'


# Main menu
script "mainmenu.rb"




script "helpers.rb"

puts "Hi from ruby!"

def Alex(text)
  puts "Alex: #{text}"
end

def Berun(text)
  puts "Berun: #{text}"
end

# [[
# Put your director's instructions and comments here.

# Alex "I will have you know that it's possible to talk like this!"
# Berun %{Sure, but like this is also "possible"...}
# Alex 'I this is also "fine".'


def ulpcss_load(sprite, layer, name)
  full_name = "image/ulpcss/#{name}"
  sprite_loadulpcss sprite, layer, full_name
end

def torso_tint(sprite, r, g, b, a = 255)
  sprite_tint_rgba sprite, SPRITELAYER_TORSO, r, g, b, a
end


def hair_tint(sprite, r, g, b, a = 255)
  sprite_tint_rgba sprite, SPRITELAYER_HAIR, r, g, b, a
end

def start_load_sprites
  puts "Loading some things and sprites"
  Thing.make(1, 300, 400, 1, 32, 32, 100)
  Sprite.make(100)
  Sprite[100].load_ulpcss(SPRITELAYER_BODY , "body/female/light.png")
  Sprite[100].load_ulpcss(SPRITELAYER_TORSO, "torso/dress_female/underdress.png")
  Sprite[100].load_ulpcss(SPRITELAYER_HAIR , "hair/female/bangslong.png")
  Sprite[100].tint_hair(0, 255, 0)
  Sprite[100].tint_torso(255, 64, 64)
  Thing[100].sprite    = Sprite[100]
  Thing[100].direction = SPRITE_SOUTH
  Thing[100].pose      = SPRITE_STAND

  Thing.make(0, 400, 400, 1, 32, 32, 101)
  Sprite.make(101)
  Sprite[101].load_ulpcss(SPRITELAYER_BODY , "body/female/dark.png")
  Sprite[101].load_ulpcss(SPRITELAYER_TORSO, "torso/dress_w_sash_female.png")
  Sprite[101].load_ulpcss(SPRITELAYER_HAIR , "hair/female/bangsshort.png")
  Sprite[101].tint_hair(255, 255, 0)
  Sprite[101].tint_torso(128,  128, 255)
  Thing[101].sprite     = Sprite[101]
  Thing[101].pose       = SPRITE_STAND
  Thing[101].direction  = SPRITE_SOUTH

  puts "Things and sprites loaded."

end


def start_load_stuff
end

def start_load_tilemap
  tilemap_fn = 'map/map_0001.tmx'
  tilemap  = Store.load_tilemap(:map_0001, tilemap_fn)
  State.tilemap_name = :map_0001
  active_map_ tilemap.id
  
end


def reload_tilemap
  active_map_(-1) # disable map
  if State.tilemap_name
    old_map = Store[State.tilemap_name]
    old_map.drop! if old_map
  end
  tilemap_fn = 'map/map_0001.tmx'
  tilemap  = Store.load_tilemap(:map_0001, tilemap_fn)
  active_map_ tilemap.id
end


def do_start_test_map
    start_load_tilemap
    start_load_sprites
    start_load_stuff
  # start_setup_ui
    actor_switch(Thing[100])
end


#
# def load_main_menu
#   $main_music   = Store.load_audio_stream('/music/hiver_com64.ogg')
#   $main_back    = Store.load_bitmap('/image/background/eruta_mainmenu.png')
#   $main_graph   = Graph.make_image(0, 0, $main_back.id)
#   Eruta::Audio.music_id = $main_music.id
#   res = Eruta::Audio.play_music
#   puts "play_music #{res} #{$main_music.id}"
# end

POEM_TEXT = "After mankind's summer, winter suddenly came"    +
            "Most reached for the stars, but some remained"   +
            "The earth scarred by the Ancients unhostpitable" +
            "The hopes low, and so much wisdom forgotten"     +
            "Yet when it seemed the last hour struck"         +
            "Our hope returned, a savior arose"               +
            "Lord Kei, who by Creator's grace"                +
            "Restored the Trees that guard us all from harm"  +
            "Thus ushered in a new millennial spring"         +
            "Lord Kei, watch over us and protect us"          +
            "Until the Travellers return with bounty of the stars"

INTRO_TEXT = "Millennia have passed since mankind first traveled to the moon. "+
             "Civilizations rose as never before, yet to fall again. " +
             "When all hope seemed lost, the 21 trees sprouted from the earth. " +
             "They brought mysterious powers that protected and strengthened mankind. "+
             "Hi!\n\n" +
             "Hello µ world, this is me, 無 life should be\nfun for everyone!"

INTRO_TEXT2 = "Something happened, and I was changed!"

def make_sub_menu(parent, x, y, w, h, bh)
  sub_menu = parent.make_menu(x, y, w, h, nil)
  sub_menu.make_button(x, y + 20, w - 20, bh, "Sub choice 1") do
    puts "choice 1"
  end
  sub_menu.make_button(x, y + 30 + bh, w - 20, bh, "Sub choice 2") do
    puts "choice 2"
  end
  sub_menu.make_button(x, y + 40 + 2* bh, w - 20, bh, "Sub choice 3") do
    puts "choice 3"
  end
  sub_menu.fit_children
  sub_menu.hide
  return sub_menu
end

def do_main_menu
  $main_music   = Store.load_audio_stream(:music_main, '/music/nethis-the_writer.ogg')
  $lote         = nil
  $lobe         = nil
  if PLAY_MUSIC
    Eruta::Audio.music_id = Store[:music_main].id
    res = Eruta::Audio.play_music
  end
  # res = nil
  # $main_menu = MainMenu.new
  # $main_menu.active = true
  Zori.make_page(:default) do |m|
    State.talk_box            = m.make_longtext(10, 310, 620, 160, "Testing 1 2 3")
    State.talk_box.graph.each { |g| g.font = Eruta::Zori.font.id }
    State.talk_box.delay      = 0.1
    State.talk_box.page_lines = 5
    State.talk_box.page       = 0
    State.talk_box.hide
  end

  
  Zori.make_page(:main_menu) do |m|
    if MAIN_BACKGROUND 
      main_back    = Store.load_bitmap(:bitmap_background,
                    '/image/background/eruta_mainmenu.png')
    end

    m.graph_image(0, 0, main_back.id)
    main_menu     = m.make_menu(250, 190, 120, 440, nil)
    ma            = main_menu

    main_button_1 = ma.make_button(260, 200, 100, 30, "Continue")
    sub_menu = make_sub_menu(main_button_1, 50, 190, 120, 440, 30)
    sub_menu.hide
    
    main_button_2 = ma.make_button(260, 240, 100, 30, "New") do
      do_start_test_map
      Zori.go(:default)
    end
    
    main_button_3 = ma.make_button(260, 280, 100, 30, "Settings") do
      Zori.go(:settings)
    end
    
    main_button_4 = ma.make_button(260, 320, 100, 30, "Instructions")
    main_button_5 = ma.make_button(260, 360, 100, 30, "µ£éè")
    main_button_5 << sub_menu
    
    main_button_5= ma.make_button(260, 400, 100, 30, "Quit") do
      Eruta.quit
    end
    
    main_menu.fit_children
  end

  Zori.make_page(:settings) do |se|
      lote2          = se.make_longtext(100, 10, 160, 100, INTRO_TEXT)
      lote2.delay    = 1
      lote2.page     = 0
      lote2.page_lines = 3
      # $lote2.line_stop = 999990
      
      settings_menu  = se.make_menu(480, 380, 120, 440, nil)
      sm             = settings_menu
      settings_ok_button = sm.make_button(500, 300, 100, 30, "Font 1") do
         if lote2
            lote2.graph.each { |g| g.font = 0 }
         end
      end
      settings_ok_button = sm.make_button(500, 350, 100, 30, "Font 2") do
         if lote2
            lote2.graph.each { |g| g.font = Eruta::Zori.font.id }
         end
         lote2.text = INTRO_TEXT2
      end
      settings_ok_button = sm.make_button(500, 400, 100, 30, "OK") do
         Zori.go(:main_menu)
         if lote
            lote.close
            lote = nil
         end
      end
      sm.fit_children
  end

  Zori[:main_menu].hide
  Zori[:settings].hide

  Zori.go(:main_menu)
end


def on_start(*args)
  puts "on_start #{args}"
  State.time_start = Eruta.time


  if START_TEST_MAP
    start_load_tilemap
    start_load_sprites
    start_load_stuff
    start_setup_ui
    actor_switch(Thing[100])
      else
    Zori.open
    do_main_menu()
  end
  return :ok
end


def on_reload(*args)
  puts "on_reload #{args}"
  puts "load thing ok: #{Thing.methods - Object.methods}"
  puts "load thing ok: #{Eruta::Thing.methods - Object.methods}"
  $t1 = Thing.make(1, 700, 700, 1, 32, 32)
  return :ok
end


def on_bump(t1, t2, kind = nil)
  puts "BUMP! #{t1} #{t2} #{kind}"
  return true
end

def ok!
  log "OK, it works!"
end


def on_update(dt)
=begin
  time_now = Eruta.time
  if (time_now - State.time_start) > 10.0
    # puts "Tick Tock goes the clock."
    # do something here if needed ... 
    State.time_start = time_now
  end
  return nil
=end
end

def actor_switch(new_actor)
  # Ensure the old actor stops walking.
  new_actor.v = [ 0.0, 0.0 ]
  Thing.actor = new_actor
  new_actor.track
  return new_actor
end

def actor_continue_talk
  if State.talk_box.at_end?
    State.talk_box.hide
    State.talk_with    = nil
    p "Talk ended."
  elsif State.talk_box.paused
    res = (State.talk_box.next_page)
    p "Continue_talk", res
  else
    State.talk_box.delay = 0.05
    p "Talking speed up"
    # speed up talk here?
  end
end

def actor_start_search
  p "starting search"
  found = Thing.actor.find_in_front(60, 60)
  unless found.empty?
    first                     = found.first
    State.talk_box.delay      = 0.05
    State.talk_box.show
    State.talk_box.text       = "Let's make this text a bit longer\n
Hi there, I'm #{first.id}!"
    State.talk_with           = first
  end
end

# Searches and interacts with things in front of the actor.
# Or if already talking/reading, continue with it.
def actor_search_or_talk
  return if !Thing.actor
  if State.talk_with
    actor_continue_talk
  else
    actor_start_search
  end
end

# Show a reminder of the key bindings on stdout
def show_keybindings
  puts "Keybindings:"
  puts "A: Switch to actor A."
  puts "B: Switch to actor B."
  puts "E: REload currently active tile map."
  puts "F: Toggle the visibility of the FPS counter"
  puts "G: Toggle the visibility of the 2D scene Graph"
  puts "H: Hide 2D scene graph GUI"
  puts "N: Disable tile map."
  puts "P: Toggle the visibility of the area/physics bounds boxes."
  puts "R: Toggle the visibility of the area spRites."
  puts "S: Show 2D scene graph GUI"
  puts "Arrow keys: Move active actor."
  puts "Space: search in front of active actor or continue talking."
end

# Handle key down
def on_key_down(time, key)

  # Filter away duplicated keystrokes. XXX: find out WHY these occur.
  if State.last_key_time
    delta = time - State.last_key_time
    if (delta < 0.00001) && State.last_key == key
      State.last_key_time = time
      State.last_key      = key
      p "Duplicated keystroke: #{time} #{key}"
      return
    end
  end
  State.last_key_time = time
  State.last_key      = key
 
  p "on_key_down #{time} #{key}"
  actor         = Thing.actor
  return nil unless actor
  vx, vy        = * actor.v
  case key
    when KEY_A
      actor_switch(Thing[100])
    when KEY_B
      actor_switch(Thing[101])
    when KEY_E
      reload_tilemap()
    when KEY_H
      Eruta::Graph.visible_(52, 0)
    when KEY_M
      active_map_($tilemap_id)
    when KEY_N
      active_map_(-1)
    when KEY_S
      Eruta::Graph.visible_(52, 1)
    when KEY_F
      Eruta.show_fps=     !Eruta.show_fps
    when KEY_R
      Eruta.show_area=    !Eruta.show_area
    when KEY_G
      Eruta.show_graph=   !Eruta.show_graph
    when KEY_P
      Eruta.show_physics= !Eruta.show_physics
    when KEY_UP
      vy -= 100.0
    when KEY_DOWN
      vy += 100.0
    when KEY_LEFT
      vx -= 100.0
    when KEY_RIGHT
      vx += 100.0
    when KEY_SPACE
      puts "Searching..."
      actor_search_or_talk
    when KEY_COMMA
      show_keybindings
    else
  end
  actor.v = [ vx, vy ]
  return nil
end

# Handle key up. Better refactor this to a game class or module
def on_key_up(time, key)
  actor         = Thing.actor
  return nil unless actor
  vx, vy        = * actor.v
  case key
  when KEY_UP
    vy = 0.0
  when KEY_DOWN
    vy = 0.0
  when KEY_LEFT
    vx = 0.0
  when KEY_RIGHT
    vx = 0.0
  when KEY_SPACE
    State.talk_box.delay = 0.1 if State.talk_with
  else
  end
  actor.v = [vx, vy]
  return nil
end


# Called when an input event occurs
def on_poll(*args)
  # Send to Zori ui first. If it returns non-nil the event is handled,
  # otherwise, pass on to key handler.
  res = Zori.on_event(*args)
  if res
    return nil
  else
    type = args.shift
    meth = "on_#{type}".to_sym
    if self.respond_to?(meth)
      self.send(meth, *args)
    else
      # ignore
      # puts "input #{type} #{meth} #{args}"
    end
  end
end


# log "this is for the console."
#
# test()
# log "Hi from Ruby! Björn"
# path = PathForData "script/main.lua"
# if path then
#   log(path:to_s())
# else
#   log("path not found")
# end
# path = nil
# collectgarbage()
#
# mainmenu_image = Image("image/background/eruta_mainmenu.png")
# if mainmenu_image then
#   local w = mainmenu_image:w()
#   local h = mainmenu_image:h()
#   log("Image dimensions:" .. w .. "," .. h)
# end
#
# font_tuffy = Font("Tuffy.ttf", 16, 0)
# if font_tuffy then
#   local w = font_tuffy:w("M")
#   local h = font_tuffy:h()
#   log("Font:" .. w .. "," .. h)
# end
#
#
#
# function on_start(s)
#   log("Start! " .. s)
# end
#
# le = nil
#
# function on_event(eve)
#   le = eve
#   local type = eve:type()
#   log("Event type: " .. type)
#   eve = nil
#   collectgarbage()
# --  io.write("Draw!\n")
# end
#
#
# function on_draw(s)
#   -- log("draw! " .. s)
#   -- log("Draw!")
# --  io.write("Draw!\n")
# end
#
#
# function on_update()
# --  io.write("Update!\n")
# end
#
#
#
#
#
#
#
#
