import xforms;

public class MainWindow
{
   Form;
    
   constructor new()
   {
      Form := (forward loader)(self)
   }
    
/*    onExit(arg)
    {
        forward program.stop()
    }*/
    
   dispatch() => Form;    
}