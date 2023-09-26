import system'drawing;

// --- BaseControl ---

public abstract class BaseControl
{
   bool   _visible;
   bool   _enabled;
   Region _region;   

   wide   _caption;

   protected constructor new()
   {
      _visible := true;
      _enabled := true;
      _region := new Region();   
      _caption := emptyWideString;
   }
}