import system'winforms;
import forms;
import system'drawing;
import opengl'externals;

const int[] pixelFormatAttributes = $[ 
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
];

const int[] openGLAttributes = $[ 
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
];

public sealed class OpenGLDisplay : interface<IOGLController>
{
   IOpenGLEngine _engine;
   OGLContext    _paintContext;
   HDC           _dc;
   HGLRC         _context;
   
   private initGL(HDC dc)
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
      
      HGLRC context := WGLLibrary.createContext(dc);
      if (context.isUnassigned()) {
         OpenGLException.raise("OpenGLWrapper: failed to create context");
      };

      context.makeCurrent(dc);

      if:not(_engine.WGL.load(dc)) {
         OpenGLException.raise("WGLContext: failed to load");
      };
      
      if:not(_engine.GL.load()) {
         OpenGLException.raise("GLContext: failed to load");
      };
      
      context.clearCurrent(dc);
      WGLLibrary.freeContext(context);
      
      (int major, int minor) := _engine.GL.getVersion();
      
      _engine.init(major, minor);
   }

   private initializeOGL()
   {
      auto dummyHandle := new WindowHandle(
                        0,
                        OGL3DDummyWindow,
                        EmptyWideString,
                        0,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        WindowHandle.Default,
                        CurrentInstance,
                        UnsafePointer.Default);
        
      HDC dummyDC := dummyHandle.HDC;
                              
      initGL(dummyDC);
      
      dummyDC.release(dummyHandle.Handle);
      dummyHandle.free();
   }

   private makeCurrent(HDC dc)
   {
      _context.makeCurrent(dc);
   }

   load(HDC dc)
   {
      _dc := dc;
      _engine.WGL.setPixelFormat(dc, pixelFormatAttributes);
      _context := _engine.WGL.createContext(dc, openGLAttributes);
      
      makeCurrent(dc);
   }

   free()
   {
      WGLLibrary.freeContext(_context);
   }

   present(bool vsync)
   {
      _engine.WGL.present(_dc, vsync);
   }

   constructor new(OGLContext paintContext, IOpenGLEngine engine)
   {
      _paintContext := paintContext;
      _engine := engine;
      
      if:not(_engine.IsInitialized) {
         initializeOGL();
      };
   }
}
