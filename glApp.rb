class GlApp < Text

  class Input
    def self.press?(key)
       "keys['#{key}']".to_expr
    end
  end

  class Audio
    def self.bgm_play(bgm)
       lambda{|scope|
          scope.mciSendString "play #{bgm.inspect}", 0, 0, 0
       }
    end
  end

  def initialize(name)
    super

    output %{
      #include <asset/glapp.inc>
      #include <mmsystem.h>
    }

    init self
  end

  
  def setglobal(name, value)
    @globals ||= {}
    @globals[name] = value
  end

  def compile
    @w = Function.new(self) 
    @w.start "void GameMain()"
    gamemain @w
    @w.close

    @globals.each{|k, v|
        output "
          int g_#{k}(){ return #{v}; }
        "
    }
    super
  end

  def gamemain writer

  end

  def init writer
    setglobal :winwidth, 640
    setglobal :winheight, 480
    setglobal :fullwidth, 1024
    setglobal :fullheight, 768
  end
end

