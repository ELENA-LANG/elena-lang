public interface IOpenGLEngine
{
   abstract setVersion(int major, int minor);
}

public interface IOpenGLDeviceAdapter
{
   abstract setup(IOpenGLEngine engine);
}

public class OpenGLEngine : interface<IOpenGLEngine>
{
   IOpenGLDeviceAdapter _adapter;
   
   int                  _major; 
   int                  _minor; 
   
   constructor new(IOpenGLDeviceAdapter adapter)
   {
      _adapter := adapter;
      _adapter.setup(self);
   }
   
   setVersion(int major, int minor)
   {
      _major := major;
      _minor := minor
   }
}