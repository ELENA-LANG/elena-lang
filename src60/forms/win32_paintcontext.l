import system'winforms;
import system'drawing;
import system'runtime;

// --- IPaintContextController ---

public interface IPaintContextController
{
   set abstract Parent(PaintContext context);
   
   abstract initializeDC(HDC dc);
}

singleton DummyController : IPaintContextController
{
   set Parent(PaintContext context) {}
   
   initializeDC(HDC dc) {}
}

// --- PaintContext ---

class PaintContextListener : WindowCallback
{
   PaintContext _owner;
    
   constructor(PaintContext owner)
   {
      _owner := owner
   }

   onSize(Handle control, int width, int height)
   {
      _owner.resize(control, width, height);
   }

   onWMPaint(Handle hwnd)
   {
      _owner.paint();
   }

   onWMDestoy(Handle hwnd)
   {
      _owner.destroy();
        
      super.onWMDestoy(hwnd);

      PermVectorTable.release(self);
   }
}

public closed class PaintContext : BaseWinControl, interface<IControl>
{
   IPaintContextController Controller : rprop(_controller);
   
   set Controller(IPaintContextController controller)
   {
      _controller := controller;
      
      _controller.Parent := self;
   }   
   
   constructor new()
      <= super new()
   {
      _controller := DummyController;
   }

   internal destroy()
   {
   }

   internal createHandle()
   {
      WindowHandle parent := _parent;

      int styles := WS_CHILD;
      if(_visible)
         { styles := styles | WS_VISIBLE };
                        
      _region.read(out int x, out int y, out int width, out int height);
       
      pointer paramPtr := PermVectorTable.allocate(new PaintContextListener(self)); 

      _handle := new WindowHandle(
                        WS_EX_TRANSPARENT,
                        PaintContextClass,
                        cast wide(_caption),
                        styles,
                        x, y, width, height,
                        parent,
                        CurrentInstance,
                        paramPtr);
                        
      _controller.initializeDC(_handle.HDC);
      
      int m := 0;
   }

   internal paint()
   {
   }   
}
