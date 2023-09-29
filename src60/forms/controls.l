import system'drawing;

// --- BaseControl ---

public abstract class BaseControl
{
   bool   _visible;
   bool   _enabled;
   Region _region;   

   wide   _caption;

   string Caption
   {
      get() = _caption;
        
      set(s)
      {
         _caption := s
      }
   }

   setRegion(int x, int y, int width, int height)
   {
      _region.set(x, y, width, height);
   }

   protected constructor new()
   {
      _visible := true;
      _enabled := true;
      _region := new Region();   
      _caption := emptyWideString;
   }
}