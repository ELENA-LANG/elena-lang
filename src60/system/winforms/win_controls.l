import system;

// --- classes ---
public const wide ButtonClass = "Button"w;
public const wide EditClass   = "Edit"w;
public const wide StaticClass = "Static"w;

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

   bool equal(Handle h)
      = _handle == h;

   handle Handle
      = _handle;

   get constructor Default()
      = new WindowHandle();

   setPosition(int x, int y, int cx, int cy, int flags)
   {
      extern USER32.SetWindowPos(_handle, 0, x, y, cx, cy, flags)
   }

   setCaption(wide s)
   {
      extern USER32.SendMessageW(_handle, WM_SETTEXT, 0, s)
   }
    
   wide getCaption()
   {
      int length := extern USER32.SendMessageW(_handle, WM_GETTEXTLENGTH, 0, 0);
        
      if (length == 0)
      { 
         ^ emptyWideString 
      }
      else
      {
         wide caption := WideString.allocate(length);
         length := length + 1;
            
         extern USER32.SendMessageW(_handle, WM_GETTEXT, length, caption);
            
         ^ caption
      }
   }
}

WindowHandle defaultHandle
    = WindowHandle.Default;

