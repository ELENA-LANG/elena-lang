import opengl'externals;

public interface IOpenGLEngine
{
   get abstract WGLLibrary WGL();
   get abstract GLADLibrary GL();
}

//public interface IOpenGLDeviceAdapter
//{
//   abstract setup(IOpenGLEngine engine);
//}

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
      
//      _adapter := adapter;
//      _adapter.setup(self);
   }
   
//   setVersion(int major, int minor)
//   {
//      _major := major;
//      _minor := minor
//   }
}