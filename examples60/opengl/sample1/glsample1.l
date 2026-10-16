import xforms;
import opengl;

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