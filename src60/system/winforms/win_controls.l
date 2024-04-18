import system;
import system'drawing;

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
      handle hParent := parent.Handle;
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
            hParent,
            0,
            hinstance,
            paramPtr)
      };

      int err := extern KERNEL32.GetLastError();

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
      };

      wide caption := WideString.allocate(length);
      length := length + 1;
            
      extern USER32.SendMessageW(_handle, WM_GETTEXT, length, caption);
            
      ^ caption
   }

   setEnabled(int value)
   {
      extern USER32.EnableWindow(_handle, value)
   }            
    
   bool isEnabled()
   {
      int visible := extern USER32.IsWindowEnabled(_handle);
        
      ^ (visible != 0)
   }

   setChecked(int n)
   {
      extern USER32.SendMessageW(_handle, BM_SETCHECK, n, 0)
   }

   bool isChecked()
   {
      int r := extern USER32.SendMessageW(_handle, BM_GETCHECK, 0, 0);

      ^ (r == BST_CHECKED)
   }    

   setVisible(int n)
   {
      extern USER32.ShowWindow(_handle, n)
   }
    
   bool isVisible()
   {
      int visible := extern USER32.IsWindowVisible(_handle);
        
      ^ (visible != 0)
   }

   get HDC HDC()
   {
      handle dc := extern USER32.GetDC(_handle);

      ^ dc
   }

   refresh()
   {
      handle copy := _handle;
        
      extern
      {
         extern USER32.InvalidateRect(copy, 0, 1);
         extern USER32.UpdateWindow(copy)
      }
   }

}

WindowHandle defaultHandle
    = WindowHandle.Default;

// --- PAINTSTRUCT ---

public sealed packed PAINTSTRUCT
{
   HDC  _HDC;
   int  _erase;
   RECT _rect;
   int  _restore;
   int  _incUpdate;
    
   int  dummy0;
   int  dummy1;
   int  dummy2;
   int  dummy3;
   int  dummy4;
   int  dummy5;
   int  dummy6;
   int  dummy7;

   constructor()
   {
   }

   get HDC HDC()
   {
      ^ _HDC
   }
            
   begin(WindowHandle hwnd)
   {
      Handle h := hwnd.Handle;

      extern USER32.BeginPaint(h, self)
   }
    
   end(WindowHandle hwnd)
   {
      Handle h := hwnd.Handle;

      extern USER32.EndPaint(h, self)
   }
}
