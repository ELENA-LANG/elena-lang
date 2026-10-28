import system'drawing;

use extern OPENGL : importing("OPENGL32");

public const struct HGLRC
{
   embeddable handle _handle;
   
   bool isUnassigned()
      = _handle.isUnassigned();
   
   constructor(handle handle)
   {
      _handle := handle
   }
   
   makeCurrent(HDC dc)
   {
      int retVal := OPENGL.wglMakeCurrent(dc, _handle);
   }
}

internal singleton WGLWrapper
{
   HGLRC createContext(HDC dc)
   {
      handle h := OPENGL.wglCreateContext(dc);
      
      ^ h
   }
}