import xforms;
import opengl;
import logger;

public symbol Initialization =
   LogSetup
      .registerFromString(__project_constants["logger"])
      .injectErrorHandler()
      .injectLogger();

public class MainWindow : XForm, using(MainWindow)
{
   OpenGLEngine _engine;
   
   constructor new()
      <= super new()
   {
      _engine := OpenGLEngine.new($());
      
      auto ogController := OpenGLPaintContextController.new(glContext, _engine);
      
      //glContext.Controller := ogController;
   }

   private onExit(sender)
   {
      forward Program.stop()
   }
}
