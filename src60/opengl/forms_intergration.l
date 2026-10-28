import forms;
import system'drawing;

public class OpenGLPaintContextController : interface<IPaintContextController>, interface<IOpenGLDeviceAdapter>
{
   //IOpenGLDeviceTarget _target;
   PaintContext        _paintContext;
   
   setup(/*IOpenGLDeviceTarget target*/)
   {
      //_target := target;
      
      initialize();
   }
/*   
   set Parent(PaintContext context)
   {
      _context := context
   }
*/   
   loadGL(HDC dc)
   {
      PIXELFORMATDESCRIPTOR pfd := default;
      pfd.nSize := :sizeof pfd;
      pfd.nVersion := 1;
      pfd.iPixelType := PFD_TYPE_RGBA;
      pfd.dwFlags := PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
      pfd.cColorBits := 32;
      pfd.cAlphaBits := 8;
      pfd.iLayerType := PFD_MAIN_PLANE;
      pfd.cDepthBits := 24;
      pfd.cStencilBits := 8;

      int pixelFormat := pfd.choosePixelFormat(dc);
      pfd.setPixelFormat(dc, pixelFormat);
      
      HGLRC context := WGLWrapper.createContext(dc);
      if (context.isUnassigned()) {
         OpenGLException.raise("OpenGLWrapper: failed to create context");
      };
      
      context.makeCurrent(dc);
      
/*
    if (!gladLoadWGL(dummyDC))
        throw std::runtime_error("OGraphicsEngine - gladLoadWGL failed");
    
    if (!gladLoadGL())
        throw std::runtime_error("OGraphicsEngine - gladLoadGL failed");
    

    wglMakeCurrent(dummyDC, 0);
    wglDeleteContext(dummyContext);

*/      
   }

   initialize()
   {
      _paintContext.initializeOGL3DDummyWindow(self);
      
      
//      
//      OpenGLWrapper.makeCurrent(dc, context);
      
/*
    HGLRC dummyContext = wglCreateContext(dummyDC);
    assert(dummyContext);

    bool res = wglMakeCurrent(dummyDC, dummyContext);
    assert(res);


    if (!gladLoadWGL(dummyDC))
        throw std::runtime_error("OGraphicsEngine - gladLoadWGL failed");
    
    if (!gladLoadGL())
        throw std::runtime_error("OGraphicsEngine - gladLoadGL failed");
    

    wglMakeCurrent(dummyDC, 0);
    wglDeleteContext(dummyContext);
    ReleaseDC(dummyWindow, dummyDC);
    DestroyWindow(dummyWindow);

*/      
   }
   
   constructor new(PaintContext paintContext)
   {
      _paintContext := paintContext
   }
}

