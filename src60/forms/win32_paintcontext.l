import system'winforms;
import system'drawing;
import system'runtime;

// --- IPaintContextController ---

public interface IPaintContextController
{
   abstract load(HDC dc);

   abstract paint(HDC dc);
}

singleton DummyController : IPaintContextController
{
   load(HDC dc) {}
   
   paint(HDC dc) {}
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

public sealed class PaintContext : BaseWinControl, interface<IControl>
{
   IPaintContextController _controller;
  
   set Controller(IPaintContextController controller)
   {
      _controller := controller;      
      //_controller.Parent := self;
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
                        
      _controller.load(_handle.HDC);
   }
   
   HDC HDC
      = _handle.HDC;
   
   internal paint()
   {
      _controller.paint(_handle.HDC);
   }
}
