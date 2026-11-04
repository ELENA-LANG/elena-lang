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



// --- GLContext ---

public struct GLContext
{
   handle libGL;
   
   constructor()
   {
      libGL := extern KERNEL32.LoadLibraryW("opengl32.dll"w);
   }

   bool load()
   {
      ^ true
   }
}
