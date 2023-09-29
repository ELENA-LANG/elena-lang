import system;

// --- classes ---
public const wide ButtonClass = "Button"w;

// --- WindowHandle ---

public sealed const struct WindowHandle
{
   handle _handle;

   constructor()
   {
      _handle := Handle.Default
   }

   constructor(handle h)
   {
      _handle := h 
   }

   constructor(int dwExStyle, wide className, wide windowName, int style, int x, int y, int width, int height, 
      WindowHandle parent, HInstance instance, pointer paramPtr)
   {
      handle hinstance := instance.Handle;
      handle h := UnsafePointer.Default;
      extern
      {
         h := extern USER32.CreateWindowExW(
            dwExStyle,
            className,
            windowName,  
            style,
            x,
            y,
            width,
            height,
            parent.Handle,
            0,
            hinstance,
            paramPtr)
      };

      if (h==0)
         { WinFormsException.raise() };

      _handle := h

   }

   handle Handle
      = _handle;

   get constructor Default()
      = new WindowHandle();

}

WindowHandle defaultHandle
    = WindowHandle.Default;

