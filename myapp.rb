class X < GlApp
  def init writer
    super
    Function.new(writer) do |func|
       func.start "void orzfly()"
       Scope.new(func).MessageBox(0, "囧叔好萌!".encode("GBK"), 0, 16).endl
    end
  end

  def gamemain writer
    Input.press?("A").if(writer) do 
       Audio.bgm_play("1.mid").call(self); endl;
       orzfly; endl;
    end
  end


end


X.new("1.cpp").compile