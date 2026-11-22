import xforms;
import forms;
import opengl;
import logger;

public symbol Initialization =
   LogSetup
      .registerFromString(__project_constants["logger"])
      .injectErrorHandler()
      .injectLogger();

public class Game
{
   OpenGLEngine Engine:rprop(_engine);

   OpenGLDisplay _display;

   constructor(PaintContext painContext)
   {
      _engine := OpenGLEngine.new($());
      
      _display := OpenGLDisplay.new(painContext, _engine);
      
      painContext.Controller := _display;
   }
   
   onUpdate()
   {
      _engine.Canvas.clear($(1.0f, 0.0f, 0.0f, 1.0f));
      _display.present(false);
   }
}

public class MainWindow : XForm, using(MainWindow)
{
   Game          _game;
   OpenGLDisplay _display;
   
   constructor new()
      <= super new()
   {
      _game := $(glContext);
      
      self.onOpen := (self){ forward Program.OnUpdate := (){ _game.onUpdate() } };
   }

   private onExit(sender)
   {
      forward Program.stop()
   }
}
