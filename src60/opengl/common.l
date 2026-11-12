import extensions;

const int GL_VERSION        = 1F02h;
const int GL_NUM_EXTENSIONS = 821Dh;

public sealed class OpenGLException : Exception
{
   constructor new(string message)
      <= super new(message)
   {
   }   
}
