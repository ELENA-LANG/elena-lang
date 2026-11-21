import forms;
import system'drawing;
import opengl'externals;

public class OpenGLPaintContextController : interface<IPaintContextController>
{
   IOpenGLEngine _engine;
   PaintContext  _paintContext;
   
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
   }

   constructor new(PaintContext paintContext, IOpenGLEngine engine)
   {
      _paintContext := paintContext;
      _engine := engine
   }
}
