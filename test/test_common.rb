$: << '../src'

require 'minitest/autorun'
require 'sdl'
require 'sisa'
require 'rogaru'

module Test
  class Common < MiniTest::Unit::TestCase
  # Utility functions for common use in tests.
  
    # Opens the screen (should be done first)
    def open_screen(wide = 640, high = 480, fullscreen  = false)
      @screen         = Sisa::Screen.make!(wide, high, fullscreen)
      @screen.caption  = 'Sisa Test'
      return @screen
    end
    
    # Opens the event queue
    def open_queue
      @queue = Sisa::Event.queue
      return @queue
    end
    
    def screen
      return @screen
    end
    
    def queue
      return @queue
    end
    
    # Transparent black is the forbidden color
    FORBIDDEN_COLOR = Sisa::Color::TILE_ANIMATION 
    # Sisa.color(0,0,0,255)
    
    # Wipes the screen to the forbidden color
    def wipe_screen
      @screen.quickfill_rectangle(0, 0, @screen.w, @screen.h, FORBIDDEN_COLOR)
    end
    
    # Draws the scene in the block. Automatically calls @screen.flip
    # after every draw, unless the last argyment is false   
    # Returns time, frames and fps
    # Stops automatically when a certain time has passed (if given)
    # or when the ESCAPE or RETURN key, or other given keys are pressed     
    def draw_wait(timeout = nil, keys = [Sisa::Key::ESCAPE, Sisa::Key::RETURN  ], flip = true)
      frames     = 0
      start      = Time.now
      busy       = true
      while busy do
        yield # (to the block)
        @screen.flip if flip
        frames  +=1
        if timeout 
          delta  = Time.now - start
          busy   = false if delta > timeout
        end
        event    = @queue.poll
        while event
          if event.is_a?(Sisa::Event::KeyDown) && keys.member?(event.sym)
            busy = false
          end   
          if event.is_a? Sisa::Event::Quit
            busy = false
          end   
          event = @queue.poll
        end  
        
      end
      stop    = Time.now
      delta   = stop - start
      fps     = frames.to_f / delta.to_f
      return frames, delta, fps
    end
    
    # Asserts that the point at x, y has been drawn and is not 
    # transparent black or the given color    
    def assert_drawn(x, y, forbidden = FORBIDDEN_COLOR)
       color = @screen.get_point(x,y)
       assert color != forbidden , "Not drawn correctly at #{x} #{y}"
    end
    
    # Asserts that the point at x, y has not been drawn to and is not 
    # transparent black or the given color    
    def assert_not_drawn(x, y, forbidden = FORBIDDEN_COLOR)
       color = @screen.get_point(x,y)
       assert color == forbidden , "Drawn unneededly at #{x} #{y}"
    end
    
    
    
    # Default setup opens the screen in @screen
    def setup
      Sisa.init_all
      # SDL.init(SDL::INIT_EVERYTHING)
      # open_screen(1280, 1024, true)
      # @1280 x 1024
      open_screen(640, 480, true)
      open_queue
    end
    
    # Default teardown stops SDL
    def teardown
      # Return to previous resolution.
      open_screen(1280, 1024, true)
      SDL.quit()
    end
    
    # Just to ensure the screen open is tested
    def test_common
      assert @screen, "Could not open screen"
      assert @queue, "Could not open event queue" 
    end
    
  end  
end  
