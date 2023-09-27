import system;

// --- WindowHandle ---

public sealed const struct WindowHandle
{
   embeddable __ptr _handle;

   constructor()
   {
      _handle := UnsafePointer.Empty
   }

   constructor(int dwExStyle, wide className, wide windowName, int style, int x, int y, int width, int height, WindowHandle parent, HInstance instance/*, object param*/)
   {
      pointer h := UnsafePointer.Empty;
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
            parent,
            0,
            instance,
            /*paramIndex*/0)
      };

      if (h==0)
         { WinFormsException.raise() };

      _handle := h

   }

   get constructor Default()
      = new WindowHandle();

}

WindowHandle defaultHandle
    = new WindowHandle();

