import system'winforms;

// --- BaseForm ---

public abstract class BaseWinForm : BaseWinContainer
{
    protected constructor new()
        <= super new()
    {
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
         { styles := styles || WS_VISIBLE };
        
      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      _handle := new WindowHandle(
                            EX_SDI_OVERLAPPEDWINDOW,
                            SDIWindowClass, 
                            cast wide(_caption), 
                            styles,
                            x, y, width, height,
                            WindowHandle.Default, 
                            CurrentInstance,
                            new SDIWindowListener(self))
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
      int styles := WS_POPUPWINDOW || WS_DLGFRAME;
      if(theVisible)
         { styles := styles || WS_VISIBLE };
        
      theRegion.read(ref int x, ref int y, ref int width, ref int height);
        
      _handle := new WindowHandle(
                            WS_EX_DLGMODALFRAME || WS_EX_CONTROLPARENT,
                            SDIWindowClass,
                            theCaption,
                            styles,
                            x, y, width, height,
                            cast WindowHandle(theParent),
                            CurrentInstance,
                            new ChildWindowListener(self));
   }

}