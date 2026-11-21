import opengl'externals;

public interface IOpenGLEngine
{
   get abstract WGLLibrary WGL();
   get abstract GLADLibrary GL();
}

public class OpenGLConfig
{
   
}

public class OpenGLEngine : interface<IOpenGLEngine>
{
//   int                  _major; 
//   int                  _minor; 
   
   GLADLibrary GL:rprop(_gladLibrary);
   
   WGLLibrary  WGL:rprop(_wglLibrary);
   
   constructor new(OpenGLConfig config)
   {
      _gladLibrary := $();
      _wglLibrary := $();
   }
   
/*   clear()
   {
      
   }*/
   
//   setVersion(int major, int minor)
//   {
//      _major := major;
//      _minor := minor
//   }
}