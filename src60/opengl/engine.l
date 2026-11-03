public interface IOpenGLDeviceAdapter
{
   abstract setup(/*IOpenGLDeviceTarget target*/);
}

/*public interface IOpenGLDeviceTarget
{
   
}*/

public class OpenGLEngine //: interface<IOpenGLDeviceTarget>
{
   IOpenGLDeviceAdapter _adapter;
   WGLContext           _wglContext; 
   
   constructor new(IOpenGLDeviceAdapter adapter)
   {
      _wglContext := new WGLContext();
      
      _adapter := adapter;
      _adapter.setup(/*self*/);
   }
}