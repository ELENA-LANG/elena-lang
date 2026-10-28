// --- Window Color Constants
public const int COLOR_BACKGROUND = 5;

// --- Raster Operation Codes ---

public const int SRCCOPY                     = 00CC0020h;

public const int IMAGE_BITMAP                = 0;

public const int LR_LOADFROMFILE             = 0010h;

public const int PFD_TYPE_RGBA               = 0;
public const int PFD_TYPE_COLORINDEX         = 1;

public const int PFD_MAIN_PLANE              = 0;
public const int PFD_OVERLAY_PLANE           = 1;
public const int PFD_UNDERLAY_PLANE          = -1;

public const int PFD_DOUBLEBUFFER            = 00000001h;
public const int PFD_STEREO                  = 00000002h;
public const int PFD_DRAW_TO_WINDOW          = 00000004h;
public const int PFD_DRAW_TO_BITMAP          = 00000008h;
public const int PFD_SUPPORT_GDI             = 00000010h;
public const int PFD_SUPPORT_OPENGL          = 00000020h;
public const int PFD_GENERIC_FORMAT          = 00000040h;
public const int PFD_NEED_PALETTE            = 00000080h;
public const int PFD_NEED_SYSTEM_PALETTE     = 00000100h;
public const int PFD_SWAP_EXCHANGE           = 00000200h;
public const int PFD_SWAP_COPY               = 00000400h;
public const int PFD_SWAP_LAYER_BUFFERS      = 00000800h;
public const int PFD_GENERIC_ACCELERATED     = 00001000h;
public const int PFD_SUPPORT_DIRECTDRAW      = 00002000h;
public const int PFD_DIRECT3D_ACCELERATED    = 00004000h;
public const int PFD_SUPPORT_COMPOSITION     = 00008000h;

// --- RECT ---

public sealed struct RECT
{
   int       _left;
   int       _top;
   int       _right;
   int       _bottom;
    
   constructor()
   {
      _left := 0;
      _top := 0;
      _right := 0;
      _bottom := 0
   }
    
   int Left
   {
      get() = _left;
        
      set(int v)
      {
         _left := v
      }
   }
    
   int Top
   {
      get() = _top;
        
      set(int v)
      {
         _top := v
      }
   }
    
   int Right
   {
      get() = _right;
        
      set(int v)
      {
         _right := v
      }
   }
    
   int Bottom
   {
      get() = _bottom;
        
      set(int v)
      {
         _bottom := v
      }
   }

   int Width
   {
      get()
      {
         int x2 := _right;
         int x1 := _left;
         int w := x2 - x1;
            
         ^ w
      }
   }

   int Height
   {
      get()
      {
         int y2 := _bottom;
         int y1 := _top;
         int h := y2 - y1;
            
         ^ h
      }
   }
}

// --- HDC ---

public sealed const struct HDC
{
   embeddable handle _handle;

   static HDC Default
      = default;

   handle Handle
      = _handle;
   
   constructor(handle h)
   {
      _handle := h
   }

   constructor newCompatible(HDC dc)
   {
      handle newDC := extern GDI32.CreateCompatibleDC(dc.Handle);

      _handle := newDC
   }
    
   setTextColor(int color)
   {
      extern GDI32.SetTextColor(_handle, color)
   }        

   setBkColor(int color)
   {
      extern GDI32.SetBkColor(_handle, color)
   }        
                        
   select(HBITMAP bitmapHandle)
   {
      extern GDI32.SelectObject(_handle, bitmapHandle.Handle)
   }        
    
   select(HPEN penHandle)
   {
      extern GDI32.SelectObject(_handle, penHandle.Handle)
   }
    
   select(HBRUSH brushHandle)
   {
      extern GDI32.SelectObject(_handle, brushHandle.Handle)
   }        
    
   copy(int destX, int destY, int width, int height, HDC dc, int sourX, int sourY, int flags)
   {
      extern GDI32.BitBlt(_handle, destX, destY, width, height, dc.Handle, sourX, sourY, flags);
   }        

   moveTo(int x, int y)
   {
      extern GDI32.MoveToEx(_handle, x, y, 0)
   }

   lineTo(int x, int y)
   {
      extern GDI32.LineTo(_handle, x, y)
   }

   drawText(wide text, int x, int y)
   {
      int length := text.Length;
        
      extern GDI32.TextOutW(_handle, x, y, text, length)
   }

   fillRectangle(int x, int y, int width, int height)
   {
      extern GDI32.Rectangle(_handle, x, y, x + width, y + height);
   }   
                        
   free()
   {
      extern GDI32.DeleteDC(_handle)
   }
}

// --- HBITMAP ---

public sealed const struct HBITMAP
{
   handle _handle;

   constructor create(HDC dc, int width, int height)
   {
      _handle := extern GDI32.CreateCompatibleBitmap(dc.Handle, width, height);
   }

   constructor load(wide path, int width, int height)
   {
      _handle := extern USER32.LoadImageW(
            0,   
            path,
            IMAGE_BITMAP,
            width, height,
            LR_LOADFROMFILE);
   }                

   handle Handle
      = _handle;
        
   select(HDC handle)
   {
      int retVal := extern GDI32.SelectObject(handle.Handle, _handle)
   }
    
   free()
   {
      extern GDI32.DeleteObject(_handle)
   }
}

// --- HPEN ---

public sealed const struct HPEN
{
   handle _handle;
    
   static HPEN Default
      = default;

   constructor(handle handle)
   {
      _handle := handle
   }

   bool IsEmpty()
      = _handle.isUnassigned();
                        
   constructor create(int style, int width, int color)
   {
      _handle := extern GDI32.CreatePen(style, width, color);
   }
        
   handle Handle
      = _handle;
        
   free()
   {
      extern GDI32.DeleteObject(_handle)
   }
}

// --- HBRUSH ---

public sealed const struct HBRUSH
{
   handle _handle;

   static HBRUSH Default
      = default;

   static HBRUSH COLOR_BACKGROUND
      = HBRUSH.load(COLOR_BACKGROUND);

   constructor(handle handle)
   {
      _handle := handle
   }

   internal constructor load(int constant)
   {
      _handle := Handle.loadDirty(constant);
   }

   constructor createSolid(int color)
   {
      _handle := extern GDI32.CreateSolidBrush(color);
   }
        
   handle Handle
      = _handle;
        
   free()
   {
      extern GDI32.DeleteObject(_handle)
   }
}

// --- PIXELFORMATDESCRIPTOR ---

public sealed struct PIXELFORMATDESCRIPTOR
{
  short nSize           : prop;
  short nVersion        : prop;
  int   dwFlags         : prop;
  byte  iPixelType      : prop;
  byte  cColorBits      : prop;
  byte  cRedBits        : prop;
  byte  cRedShift       : prop;
  byte  cGreenBits      : prop;
  byte  cGreenShift     : prop;
  byte  cBlueBits       : prop;
  byte  cBlueShift      : prop;
  byte  cAlphaBits      : prop;
  byte  cAlphaShift     : prop;
  byte  cAccumBits      : prop;
  byte  cAccumRedBits   : prop;
  byte  cAccumGreenBits : prop;
  byte  cAccumBlueBits  : prop;
  byte  cAccumAlphaBits : prop;
  byte  cDepthBits      : prop;
  byte  cStencilBits    : prop;
  byte  cAuxBuffers     : prop;
  byte  iLayerType      : prop;
  byte  bReserved       : prop;
  int   dwLayerMask     : prop;
  int   dwVisibleMask   : prop;
  int   dwDamageMask    : prop;
  
  int choosePixelFormat(HDC dc)
  {
     int pixelFormat := extern GDI32.ChoosePixelFormat(dc, self);
          
     ^ pixelFormat
  }
  
  int setPixelFormat(HDC dc, int format)
  {
     int retVal := extern GDI32.SetPixelFormat(dc, format, self);
     
     ^ retVal
  }
}
