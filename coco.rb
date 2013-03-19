ENV['path'] = "#{ENV['path']};C:\\railsinstaller\\DevKit\\mingw\\bin"

class Expr
  attr_accessor :obj, :scope
  def initialize(obj, scope = nil)
    @obj = obj
    @scope = scope
  end
  
  alias to_s obj
  
  [:+, :-, :*, :/, :==, :!=, :%, :^, :&, :|].each{|x|
     define_method(x){|obj|
        a = obj.is_a?(Expr) ? obj.obj : obj
        self.class.new("(#{@obj} #{x} #{a})", @scope)
     }
  }
  
  def get(obj)
      a = obj.is_a?(Expr) ? obj.obj : obj
      self.class.new("(#{@obj}.#{a})", @scope)
  end

  def rget(obj)
      a = obj.is_a?(Expr) ? obj.obj : obj
      self.class.new("(#{@obj}->#{a})", @scope)
  end

  alias [] rget

  def if(falsepart = nil, &block)
    If.new(@scope) do |x|
      x.cond << @obj
      Scope.new(x.truepart).instance_eval &block
      Scope.new(x.falsepart).instance_eval &falsepart if falsepart
    end
    self
  end
  
  def while
    While.new(@scope) do |x|
      x.cond << @obj
      Scope.new(x.yieldpart).instance_eval &block
    end
    self
  end

  def call(*args)
#Funcall.new(@scope).call(@obj, *args)
    self.class.new "#{@obj}(#{args.map{|x| Translate[x]}.join(',')})", @scope
  end

  def ret
    @scope << "return #{@obj}; \n"
  end

  def put
    @scope << @obj
    self
  end

  def putendl
    put
    endl
  end

  
  def endl
    @scope << ';'
    self
  end
end

class Scope < BasicObject
  def writer
    @writer
  end

  def initialize(writer)
    @writer = writer
  end

  def method_missing(sym, *args)
    if args == []
      sym.to_s.to_expr(self)
    else
      sym.to_s.to_expr(self).call(*args).putendl
    end
  end

  def <<(text)
    @writer << text
  end

  def expr(x)
    x.to_expr(self)
  end

  def ret(x) 
    expr(x).ret
  end

  def endl
    @writer << ";"
    self
  end

end

class Object
  def to_expr(scope)
    Expr.new(self, scope)
  end
end



class Text
  CC   = "gcc"
  CXX  = "g++"
  CFLAGS = ""
  CXXFLAGS = ""
  LDFLAGS = ""

  def initialize(filename)
    @filename = filename
    open(@filename, 'w'){|f| f.write header}
    @scopes = []
  end
  
  def header
    "#include <cstdio>\n"
  end
  
  def <<(text)
    open(@filename, 'a'){|f| f.write text}
    self
  end
  
  def compile(output = nil)
    system "#{self.class.const_get :CXX} #{self.class.const_get :CXXFLAGS} #{@filename} -o #{output || @filename + ".exe"} #{self.class.const_get :LDFLAGS}"  
  end
  
  alias output <<
end

class Writer
  def self.textpart(*syms)
    syms.each{|sym|
      class_eval %{
       def #{sym}
         Writer.new(@#{sym}||= "")
       end
      }
    }
  end
    
  def initialize(obj)
    @obj = obj
    init
    if block_given?
      yield self
      close
    end
  end
  def close
    
  end
  def init
  end
  def <<(text)
    @obj << text
    self
  end
  def endl
    output ';'
    self
  end
  alias output <<
end


class Function < Writer
  def init
    
  end
  def start(signature)
    output signature 
    output "{\n"
  end
  
  def close
    output "}"
  end
  
  def endl
    output ";"
  end

end

class Translate
  def self.[](x)
    case x
      when Symbol
         x.to_s
      else
         x.inspect
    end
  end
end

class Control < Writer
  def call(sym, *args)
    output "#{sym} #{args.map{|x| Translate[x]}.join(' ')}"
    self
  end
  alias method_missing call
end

class Funcall < Writer
  def call(sym, *args)
    output "#{sym}(#{args.map{|x| Translate[x]}.join(',')})"
    self
  end
  alias method_missing call
end

class Block < Writer
  textpart :sig, :block
  def close
     output "#{@sig}{\n#{@block}\n}"
  end
end

class If < Writer 
  textpart :cond, :truepart, :falsepart
  def close
    output "if (#{@cond}){
      #{@truepart}
    }else{
      #{@falsepart}
    }"
  end
end

class While < Writer 
  textpart :cond, :yieldpart
  def close
    output "while (#{@cond}){
      #{@yieldpart}
    }"
  end
end



$: << "."
ARGV.each{|x|
  require x
}
