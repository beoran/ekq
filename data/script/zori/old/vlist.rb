module Zori
  
  # A Vlist is a vertical list of elements.
  class Vlist
    include Graphic
    include Component
    include Handler
    
    def initialize(x, y, w, h, heading=nil)
      super(x, y, w, h)
      @heading = heading
#       @bg      = graph_box(x, y, w, h)
#       @bg.border_thickness = 0
#       @bg.border_color     = [255, 255, 255, 128]
#       @bg.background_color = BUTTON_BACKGROUND
      @tg = nil
      if @heading
        @tg                  = graph_text(x + 1, y + 1, heading)
        @tg.text_flags        = Eruta::ALIGN_LEFT
        @tg.font             = Eruta::Zori.font.id
        @tg.background_color = [0,0,0]
        @tg.color            = [255,255, 255]
      end
      if block_given?
        yield self
      end
      @state.set(:hover, :false)
    end

    
    def on_mouse_in(x, y, from)
      super(x, y)
      @pushed  = false # need to check for mouse pressed or not
    end
    
    def on_mouse_out(x, y, to)
      super(x, y)
      @pushed  = false
    end
    
    def on_mouse_axes(t, x, y, z, w, dx, dy, dz, dw)
      # Check for hovering.
      if @bounds.inside?(x, y)
        @bg.border_thickness = 1
        @bg.background_color = HOVER_BACKGROUND
        @state.set(:hover, :true)
      else
        @bg.border_thickness = 0
        @bg.background_color = BUTTON_BACKGROUND
        @state.set(:hover, :false)
      end
      return false # don't consume the event
    end
    
    def on_mouse_button_down(t, x, y, z, w, b)
      return false unless @bounds.inside?(x, y)
      if @action
        @action.call(self)
      else
        puts "Click! #{@heading}"
      end
    end
    
    def can_drag?
      return false
    end
    

    
  end
end