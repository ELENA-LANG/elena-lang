import system'winforms;
import system'runtime;

// --- BaseForm ---

public abstract class BaseWinForm : BaseWinContainer
{
   protected constructor new()
      <= super new()
   {
   }

   appendControl(BaseControl control)
   {
      control.Parent := self;
        
      _controls.append(control)
   }

   close()
   {
   }
}

// --- SDI ---

class SDIWindowListener : SDIWindowCallback
{
   SDIForm _owner;

   onClick(Handle handle)
   {
      auto control := _owner.retrieve(handle);
      if(nil != control)
         { control.click() };
        
      super.onClick(handle)
   }

   onWMDestoy(Handle hwnd)
   {
      super.onWMDestoy(hwnd);

      //TODO : free PermVectorTable entry to reuse it
   }

   onSize(Handle control, int width, int height)
   {
      _owner.resize(control, width, height);
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

// --- Child ---

class ChildWindowListener : ChildWindowCallback
{
    ChildForm _owner;
    
    constructor(ChildForm owner)
    {
        _owner := owner
    }
    
   onClick(Handle handle)
   {
      auto control := _owner.retrieve(handle);
      if(nil != control)
         { control.click() };
        
      super.onClick(handle)
   }

   onWMDestoy(Handle hwnd)
   {
      super.onWMDestoy(hwnd);

      //TODO : free PermVectorTable entry to reuse it
   }

   onSize(Handle control, int width, int height)
   {
      _owner.resize(control, width, height);
   }
    
//    onchange(WindowHandle hwnd, ref int retVal)
//    {
//        auto control := theOwner.retrieve(handle);
//        if(nil != control)
//            { control.click() };
//        
//        super.onchange(handle, ref retVal)
//    }
}

public closed class ChildForm : BaseWinForm
{
   constructor new(parent)
      <= super new()
   {
      _parent := parent
   }
    
   run()
   {
      self.open();
      _parent.Enabled := false;
        
      forward program.run(_handle);
        
      _parent.Enabled := true
   }    

   close()
   {
      self.Visible := false;
        
      super.close()
   }

   internal createHandle()
   {
      int styles := WS_POPUPWINDOW | WS_DLGFRAME;
      if(_visible)
         { styles := styles | WS_VISIBLE };

      _region.read(ref int x, ref int y, ref int width, ref int height);
        
      pointer paramPtr := PermVectorTable.allocate(new ChildWindowListener(self));

      _handle := new WindowHandle(
                            WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT,
                            SDIWindowClass,
                            cast wide(_caption), 
                            styles,
                            x, y, width, height,
                            cast WindowHandle(_parent),
                            CurrentInstance,
                            paramPtr);
    }

}

