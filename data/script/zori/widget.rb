module Zori
  # A widget may have sub-widgets
  module Widget
    include Graphic
    include State
    
    # Position of the widget.
    attr_reader :x
    attr_reader :y
    attr_reader :z
    # Dimensions of the widget.
    attr_reader :w
    attr_reader :h 
    
    # Style of the widget
    attr_reader :style
    
    # Capabilities of the widget
    attr_reader :can
    
    # State of the widget
    attr_reader :state

    # Parent of a widget
    attr_accessor :parent
    
    def initialize(params={}, &block)
      super(params, &block)
      @action            = block
      @components        = []
      @x                 = params[:x] || 0
      @y                 = params[:y] || 0
      @w                 = params[:w] || 640
      @h                 = params[:h] || 480
      self.init_graph
      @z                 = params[:z] || 0
      @style             = Zori::Style.default.dup
      # deep copy the default style
      @can               = Zori::Capability.new
      @state             = :active
      @parent            = nil
      # Widget is active
      # @action       = action
    end

    # Helper dimension, equal to x + w
    def left
      @x + @w
    end

    # Helper dimension, equal to y + h
    def bottom
      @y + @h
    end

    # Called when an event comes in from the Eruta engine.
    # Tries every component of this widget in order and stops if a truthy 
    # value is returned. Finally calls handle_event on self in case 
    # no component handles the event. 
    def on_event(*data)
      return false if disabled? || hidden?
      if @components 
        @components.each do | component |
          res = component.on_event(*data)
          return res if res
        end
      end
      res = handle_event(*data)
    end
    
    # The widget's own event handler will try to call a method named on_eventname
    def handle_event(*args)
      type   = args.shift 
      return false unless type
      method = "on_#{type}".to_sym
      # Try direct handler call.
      if self.respond_to?(method)
        return self.send(method, *args)
      else 
        return nil
      end
    end

    # Adds a component to this widget
    # Will try to send a on_component event with 
    def <<(component)
      component.parent = self
      @components << component
      self.on_event(:component, component)
    end
    
    # Checks if the coordinates are inside the main rectange of the widget.
    def inside?(x, y)
      return (x >= @x) && (x <= (@x + w)) && (y >= @y) && (y <= (@y + h))
    end
    
    
    # Makes the widget fit all direct children with the given margins
    def fit_children(margin_x = 20, margin_y = 20)
      min_x, min_y, = 640, 480 # XXX get this from somewhere...
      max_y, max_x = 0, 0 
      @components.each do | child | 
        min_x = child.x if child.x < min_x
        min_y = child.y if child.y < min_y
        max_x = child.left   if child.left   > max_x
        max_y = child.bottom if child.bottom > max_y
      end
      @x = min_x - margin_x
      @y = min_y - margin_y
      @w = max_x - min_x + margin_x * 2
      @h = max_y - min_y + margin_y * 2
      on_resize     
    end
    
    # Adds a menu to this widget as a child widget     
    def make_menu(x, y, w, h, heading, &block)
      menu = Zori::Menu.new(:x => x, :y => y, :w => w, :h => h, :heading => heading, &block)
      self << menu
      return menu
    end
    
    # Adds a button to this widget as a child widget
    def make_button(x, y, w, h, heading, &block)
      button = Zori::Button.new(:x => x, :y => y, :w => w, :h => h, :heading => heading, &block)
      self << button
      return button
    end

   # Adds a longtext to this widget as a child widget
    def make_longtext(x, y, w, h, text, &block)
      lt = Zori::LongText.new(:x => x, :y => y, :w => w, :h => h, :text => text,
                              &block)
      self << lt
      return lt
    end

    # Triggers the widget and call its action block
    def trigger
      return @action.call if @action
      return nil
    end
    
  end
end
