public interface IOpenGLDeviceAdapter
{
   abstract assign(IOpenGLDeviceTarget target);
}

public interface IOpenGLDeviceTarget
{
   
}

public class OpenGLEngine : interface<IOpenGLDeviceTarget>
{
   IOpenGLDeviceAdapter _adapter;
   
   constructor new(IOpenGLDeviceAdapter adapter)
   {
      _adapter := adapter;
      _adapter.assign(self);
   }
}