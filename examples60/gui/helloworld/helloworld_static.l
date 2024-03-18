import xforms;

public class MainWindow
{
   Form;
    
   constructor new()
   {
      Form := xforms.execute(layout, self);
   }
    
/*    onExit(arg)
    {
        forward program.stop()
    }*/
    
   dispatch() => Form;    
}