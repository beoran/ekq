# encoding: UTF-8
# Zori: 
#
# Yet another  GUI library for Ruby/SDL ( though Gari )
# beoran@gmail.com, 2009, 2010
#
# Zori (japanese footwear) follows the Shoes philosophy in that it sees width 
# as largely static, and height as dynamic and scrollable because of the mouse 
# wheel and pageup/pagedown keys
#
# There is only one top-level widget, however, every widget can contain 
# any amout of child widgets. Widgets that are children of the same parent 
# widget are said to be on the same level. 
#
#
# The box model used in Zori is as follows: 
#  ......................................
#  .margin                              .   
#  .  +-------------border-----------+  . 
#  .  | padding                      |  .
#  .  |  ..........................  |  .
#  .  |  .                        .  |  . 
#  .  |  . contents               .  |  .
#  .  |  .                        .  |  .
#  .  |  ..........................  |  .
#  .  | padding                      |  . 
#  .  +------------------------------+  .
#  .margin                              .
#  ......................................
#
# The padding determines the minimal distance between the border or the 
# parent widget and it's contents and/or child widgets.
#
# The border's thickness is only relevant for visual effects. It does not change  
# the layout. The border is effectively "inside" the padding of the widget.
#
# The margin of a widget determines how closely that widget may be packed 
# to it's sibling widgets.
# 
# The work in Zori is divided between Hanao and Widget. The Hanao class 
# handles everything that depends on and/or may influence several widgets at 
# once, such as event dispatching but also setting the focus, determining which 
# widget is being hovered, or dragged, etc. The latter fuctions change the state 
# of several widgets, so they are handled on the level of the system.
# The Widget class and it's child classes handle the individual state and
# actions of the various widgets individually.
#


module Zori
  DEBUG   = true

  autoload :Button      , 'zori/button'
  autoload :Checkbox    , 'zori/checkbox'
  autoload :Dialog      , 'zori/dialog'
  autoload :Flow        , 'zori/flow'
  autoload :Flowlayout  , 'zori/flowlayout'
  autoload :Frame       , 'zori/frame'
  autoload :Input       , 'zori/input'
  autoload :Label       , 'zori/label'
  autoload :Mainwidget  , 'zori/mainwidget'
  autoload :Menu        , 'zori/menu'
  autoload :Menubar     , 'zori/menu'
  autoload :Menuitem    , 'zori/menuitem'
  autoload :Progress    , 'zori/progress'
  autoload :Radiobutton , 'zori/radiobutton'
  autoload :Ring        , 'zori/ring'
  autoload :Sizemixin   , 'zori/sizemixin'
  autoload :Slider      , 'zori/slider'
  autoload :Stack       , 'zori/stack'
  autoload :Stacklayout , 'zori/stacklayout'
  autoload :Style       , 'zori/style'
  autoload :Submenu     , 'zori/menu'
  autoload :Hanao       , 'zori/hanao'
  autoload :Text        , 'zori/text'
  autoload :Widget      , 'zori/widget'
  autoload :Shortcut    , 'zori/shortcut'
  autoload :Mapeditor   , 'zori/mapeditor'
  
  def self.open(screen, queue) 
    return Zori::Hanao.new(screen, queue)
  end
  
end




