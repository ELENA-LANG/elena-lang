import xforms;

public class MainWindow : XForm, using(MainWindow)
{
   constructor new()
      <= super new()
   {
   }

   private onExit(sender)
   {
      forward Program.stop()
   }
}