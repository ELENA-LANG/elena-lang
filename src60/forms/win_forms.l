import system'winforms;
import system'runtime;

// --- BaseForm ---

public abstract class BaseWinForm : BaseWinContainer
{
    protected constructor new()
        <= super new()
    {
    }
}

// --- SDI ---

class SDIWindowListener : SDIWindowCallback
{
   SDIForm _owner;

   onWMDestoy(Handle hwnd)
   {
      super.onWMDestoy(hwnd);

      //TODO : free PermVectorTable entry to reuse it
   }

   constructor(SDIForm owner)
   {
      _owner := owner
   }
}

public closed class SDIForm : BaseWinForm
{
   constructor new()
      <= super new()
   {
   }

   internal createHandle()
   {
      int styles := WS_OVERLAPPEDWINDOW;
      if(_visible)
         { styles := styles | WS_VISIBLE };
        
      _region.read(ref int x, ref int y, ref int width, ref int height);

      pointer paramPtr := PermVectorTable.allocate(new SDIWindowListener(self));

      _handle := new WindowHandle(
                            EX_SDI_OVERLAPPEDWINDOW,
                            SDIWindowClass, 
                            cast wide(_caption), 
                            styles,
                            x, y, width, height,
                            WindowHandle.Default, 
                            CurrentInstance,
                            paramPtr)
   }
}

// --- SDIDialog ---

public class SDIDialog : SDIForm
{
   constructor new()
      <= super new()
   {
   }

   internal createHandle()
   {
      int styles := WS_DIALOGWINDOW;
      if(_visible)
         { styles := styles | WS_VISIBLE };
        
      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      pointer paramPtr := PermVectorTable.allocate(new SDIWindowListener(self));

      _handle := new WindowHandle(
                            EX_SDI_OVERLAPPEDWINDOW,
                            SDIWindowClass,
                            cast wide(_caption),
                            styles,
                            x, y, width, height,
                            WindowHandle.Default,
                            CurrentInstance,
                            paramPtr);
   }

}