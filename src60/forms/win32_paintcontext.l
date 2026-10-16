import system'winforms;
import system'drawing;
import system'runtime;

// --- IPaintContextController ---

public interface IPaintContextController
{
   abstract loadGL(HDC dc);
//   set abstract Parent(PaintContext context);
   
//   abstract initializeDC(HDC dc);
}

//singleton DummyController : IPaintContextController
//{
//   set Parent(PaintContext context) {}
   
//   initializeDC(HDC dc) {}
//}

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
   //IPaintContextController Controller : rprop(_controller);
/*   
   set Controller(IPaintContextController controller)
   {
      _controller := controller;
      
      _controller.Parent := self;
   }   
*/   
   constructor new()
      <= super new()
   {
      //_controller := DummyController;
   }

   initializeOGL3DDummyWindow(IPaintContextController controller)
   {
/*      
    WNDCLASSEX windowClass = {};
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = DefWindowProcA;
    windowClass.lpszClassName = L"OGL3DDummyWindow";
    windowClass.cbSize = sizeof(WNDCLASSEX);

    auto classId = RegisterClassEx(&windowClass);

    HWND dummyWindow = CreateWindowEx(
        0,
        MAKEINTATOM(classId),
        L"OGL3DDummyWindow",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        windowClass.hInstance,
        0);
*/

      auto dummyHandle := new WindowHandle(
                        0,
                        OGL3DDummyWindow,
                        EmptyString,
                        0,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        nil,
                        CurrentInstance,
                        nil);
        
      HDC dummyDC := dummyHandle.HDC;
                              
/*
    assert(dummyWindow);

    HDC dummyDC = GetDC(dummyWindow);

*/      
      controller.loadGL(dummyDC);
      
      
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
   }

   internal paint()
   {
   }   

   
}
