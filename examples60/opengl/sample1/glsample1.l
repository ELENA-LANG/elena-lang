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
      auto ogController := OpenGLPaintContextController.new(glContext);
      
      //glContext.Controller := ogController;
      _engine := OpenGLEngine.new(ogController);
   }

   private onExit(sender)
   {
      forward Program.stop()
   }
}
