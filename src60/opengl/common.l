import extensions;

const int GL_VERSION = 1F02h;

public sealed class OpenGLException : Exception
{
   constructor new(string message)
      <= super new(message)
   {
   }   
}
