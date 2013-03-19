class HGEApp < Text
  LDFLAGS = "-Iasset/hge/include -Lasset/hge/lib -lopengl32 -lkernel32 -luser32 -lgdi32 -lhge"
  

  def push scope
    @scopes.push scope
  end

  def pop
    @scopes.pop
  end


  def current &block
    if block_given?
      @scopes[-1].instance_eval &block
    else
      @scopes[-1]
    end
  end

  def ret val
    current << "return #{val};\n"
  end
 
  class MyObject
    attr_accessor :type, :name
    def initialize(type, name, obj)
       @type, @name, @obj = type, name, obj
    end

    def method_missing(sym, *args)
       @name.to_expr(@obj.current).get(sym).call(@obj.current, *args)
       @obj.current.endl
       @obj.current << "\n"
    end
  end

  def create_object(type, name)
     @list ||= []
     @list.push MyObject.new(type, name, self)
     @list[-1]
  end

  def initialize(name)
    super
    @globals = {}
    output File.read "asset/hgemain.cpp"
    @hge       = Expr.new('hge', self)
    @ctrl      = Control.new(self.current)
  end


  def runfunc(sig, local, defret = "")
    Function.new(self) do |func|    
        func.start sig
        push Scope.new(func)
        send local if respond_to? local
        pop
        func << " #{defret} "
    end
  end

  def compile 
    @list.each{|x|
        output "#{x.type} #{x.name};\n"
    }
    runfunc "void init()", :init
    runfunc "bool FrameFunc()", :render, "return false;" 
    @globals.each{|k, v|
        output "
          int g_#{k}(){ return #{v}; }
        "
    }

    system "#{self.class.const_get :CXX} #{self.class.const_get :CXXFLAGS} #{@filename} -o output/hge/#{@filename + ".exe"} #{self.class.const_get :LDFLAGS}"  
  
  end

end
