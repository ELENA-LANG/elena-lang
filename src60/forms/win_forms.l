import system'winforms;
import system'runtime;

// --- BaseForm ---

public abstract class BaseWinForm : BaseWinContainer
{
   protected constructor new()
      <= super new()
   {
      _region.X := CW_USEDEFAULT;
      _region.Y := CW_USEDEFAULT;
   }

   appendControl(BaseControl control)
   {
      control.Parent := self;

      _controls.append(weak control as:IControl)
   }

   close()
   {
   }
}

// --- SDI ---

class SDIWindowListener : SDIWindowCallback, using(ListenerBase)
{
   SDIForm _owner;

   onWMDestoy(Handle hwnd)
   {
      super.onWMDestoy(hwnd);

      PermVectorTable.release(self);
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
      onInit();
   }

   onInit() {}

   internal createHandle()
   {
      int styles := WS_OVERLAPPEDWINDOW;
      if(_visible)
         { styles := styles | WS_VISIBLE };
        
      _region.read(out int x, out int y, out int width, out int height);

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
        
      _region.read(out int x, out int y, out int width, out int height);
        
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

class ChildWindowListener : ChildWindowCallback, using(ListenerBase)
{
   ChildForm _owner;
    
   constructor(ChildForm owner)
   {
      _owner := owner
   }

   onWMDestoy(Handle hwnd)
   {
      super.onWMDestoy(hwnd);

      PermVectorTable.release(self);
   }

   onSize(Handle control, int width, int height)
   {
      _owner.resize(control, width, height);
   }
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
      _parent!.Enabled := false;
        
      forward program.run(_handle);
        
      _parent!.Enabled := true
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

      _region.read(out int x, out int y, out int width, out int height);
        
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
