import system'winforms;
import system'drawing;
import system'runtime;

// --- IPaintContextController ---

public interface IOGLController
{
   abstract load(HDC dc);
}

singleton DummyController : IOGLController
{
   load(HDC dc) {}
}

// --- PaintContext ---

class OGLContextListener : WindowCallback
{
   OGLContext _owner;
    
   constructor(OGLContext owner)
   {
      _owner := owner
   }

   onSize(Handle control, int width, int height)
   {
      _owner.resize(control, width, height);
   }

   onWMDestoy(Handle hwnd)
   {
      _owner.destroy();
        
      super.onWMDestoy(hwnd);

      PermVectorTable.release(self);
   }
}

public sealed class OGLContext : BaseWinControl, interface<IControl>
{
   IOGLController _controller;
  
   set Controller(IOGLController controller)
   {
      _controller := controller;      
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
       
      pointer paramPtr := PermVectorTable.allocate(new OGLContextListener(self)); 

      _handle := new WindowHandle(
                        WS_EX_TRANSPARENT,
                        OGLContextClass,
                        cast wide(_caption),
                        styles,
                        x, y, width, height,
                        parent,
                        CurrentInstance,
                        paramPtr);
                        
      _controller.load(_handle.HDC);
   }
}
