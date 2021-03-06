# My own version of the forwardable mixin in with the def_delegator
# However it must be used with extend, not include
module Forwardable
  # Forwards methods to the given named instance
  def def_delegator(obj_name, name, alias_name = nil)
    alias_name ||= name
    define_method(alias_name)  do |*args|
      obj = self.instance_variable_get(obj_name)
      obj.send(name, *args)
    end
  end
end

