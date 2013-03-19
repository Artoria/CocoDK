ENV['path'] = "#{ENV['path']};C:\\railsinstaller\\DevKit\\mingw\\bin"
CC   = "gcc"
CXX  = "g++"
CFLAGS = ""
CXXFLAGS = ""
LDFLAGS = "-I. -lopengl32 -lkernel32 -luser32 -lgdi32 -lglu32 -lwinmm"

class Expr
  attr_accessor :obj
  def initialize(obj)
    @obj = obj
  end
  
  alias to_s obj
  
  [:+, :-, :*, :/, :==, :!=, :%, :^, :&, :|].each{|x|
     define_method(x){|obj|
        a = obj.is_a?(Expr) ? obj.obj : obj
        Expr.new("(#{@obj} #{x} #{a})")
     }
  }
  
  def if(obj, falsepart = nil, &block)
    If.new(obj) do |x|
      x.cond << @obj
      Scope.new(x.truepart).instance_eval &block
      Scope.new(x.falsepart).instance_eval &falsepart if falsepart
    end
    self
  end
  
  def while(obj)
    While.new(obj) do |x|
      x.cond << @obj
      Scope.new(x.yieldpart).instance_eval &block
    end
    self
  end

  def call(obj, *args)
    Funcall.new(obj).call(@obj, *args)
    self
  end
  
  def endl(obj)
    obj << ';'
    self
  end
end

class Scope
  def initialize(writer)
    @writer = writer
  end
  def method_missing(sym, *args)
    sym.to_s.to_expr.call(@writer, *args)
    self
  end
  def endl
    @writer << ";"
    self
  end
end

class Object
  def to_expr
    Expr.new(self)
  end
end



class Text
  def initialize(filename)
    @filename = filename
    open(@filename, 'w'){|f| f.write header}
  end
  
  def header
    "#include <cstdio>\n"
  end
  
  def <<(text)
    open(@filename, 'a'){|f| f.write text}
    self
  end
  
  def compile(output = nil)
    system "#{CXX} #{CXXFLAGS} #{@filename} -o #{output || @filename + ".exe"} #{LDFLAGS}"  
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

class Main < Writer
  def init
    output "int main(){"
  end
  
  def close
    output "return 0;}"
  end
  
  def endl
    output ";"
  end
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

class Funcall < Writer
  def call(sym, *args)
    output "#{sym}(#{args.map{|x| x.inspect}.join(',')})"
    self
  end
  alias method_missing call
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