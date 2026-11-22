import opengl'common;
import opengl'externals;

public interface IOpenGLEngine
{
   get abstract bool IsInitialized();
   
   get abstract WGLLibrary WGL();
   get abstract GLADLibrary GL();
   
   abstract init(int major, int minor);
}

public class OpenGLConfig
{
   
}

public class OpenGLEngine : interface<IOpenGLEngine>
{
   bool        _initialized;   
   int         _major; 
   int         _minor; 
   
   IGLCanvas   _canvas;
   
   GLADLibrary GL:rprop(_gladLibrary);
   
   WGLLibrary  WGL:rprop(_wglLibrary);
   
   constructor new(OpenGLConfig config)
   {
      _initialized := false;
      
      _gladLibrary := $();
      _wglLibrary := $();
   }
   
   IGLCanvas Canvas
      = _canvas ?? _canvas := _gladLibrary.newCanvas();
   
   bool IsInitialized
      = _initialized;
   
   init(int major, int minor)
   {
      _initialized := true;
      
      _major := major;
      _minor := minor
   }
}