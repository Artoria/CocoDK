class X < GlApp
  def init writer
    super
  end

  def gamemain writer
    Input.press?("A").if(writer) do 
       Audio.bgm_play("1.mid").call(self); endl;
    end
  end


end


X.new("1.cpp").compile
