class X < HGEApp
  def initialize(name)
     super
     output %{ 
              #include <list>
              using std::list;
            }
     @a = create_object "list<int>", "a"
  end

  def init
     @hge["System_SetState"].(:HGE_SCREENWIDTH, 1200).putendl
     @hge["System_SetState"].(:HGE_SCREENHEIGHT, 600).putendl
  end
    
  def render
    current do
      puts "Hello world"
      hge["Input_GetKeyState"].(:HGEK_ESCAPE).if do
         ret 'true'
      end
    end
  end
end

X.new("1.cpp").compile
