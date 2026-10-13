import xforms;
import opengl;

public class MainWindow : XForm, using(MainWindow)
{
   OpenGLEngine _engine;
   
   constructor new()
      <= super new()
   {
      auto ogController := new OpenGLPaintContextController();
      
      glContext.Controller := ogController;
      _engine := OpenGLEngine.new(ogController);
   }

   private onExit(sender)
   {
      forward Program.stop()
   }
}