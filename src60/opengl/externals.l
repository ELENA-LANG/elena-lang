import system'drawing;

use extern OPENGL : importing("OPENGL32");

public const struct HGLRC
{
   handle _handle;
   
   bool IsEmpty()
      = _handle.isUnassigned();
   
   constructor(handle handle)
   {
      _handle := handle
   }   
}

internal singleton WGLWrapper
{
   HGLRC createContext(HDC dc)
   {
      handle h := OPENGL.wglCreateContext(dc);
      
      //int err := extern KERNEL32.GetLastError();
      
      ^ h
   }
/*   
   makeCurrent(HDC dc, HGLRC glrc)
   {
      OPENGL.wglMakeCurrent(dc, glrc)
   }
*/   
}