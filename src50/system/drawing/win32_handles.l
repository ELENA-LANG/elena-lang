// --- Raster Operation Codes ---

public const int SRCCOPY          = 00CC0020h;

public const int IMAGE_BITMAP     = 0;

public const int LR_LOADFROMFILE  = 0010h;

// --- RECT ---

public sealed struct RECT
{
    int       theLeft;
    int       theTop;
    int       theRight;
    int       theBottom;
    
    embeddable constructor new()
    {
        theLeft := 0;
        theTop := 0;
        theRight := 0;
        theBottom := 0
    }
    
    int Left
    {
        get() = theLeft;
        
        set(int v)
        {
            theLeft := v
        }
    }
    
    int Top
    {
        get() = theTop;
        
        set(int v)
        {
            theTop := v
        }
    }
    
    int Right
    {
        get() = theRight;
        
        set(int v)
        {
            theRight := v
        }
    }
    
    int Bottom
    {
        get() = theBottom;
        
        set(int v)
        {
            theBottom := v
        }
    }

    int Width
    {
        get()
        {
            int x2 := theRight;
            int x1 := theLeft;
            int w := x2 - x1;
            
            ^ w
        }
    }

    int Height
    {
        get()
        {
            int y2 := theBottom;
            int y1 := theTop;
            int h := y2 - y1;
            
            ^ h
        }
    }
}

// --- HDC ---

public sealed const struct HDC
{
    embeddable __raw theHandle[4];

    static HDC Default
        = 0;

    embeddable constructor(int handle)
    {
        theHandle := handle
    }

    embeddable constructor newCompatible(HDC dc)
    {
        int newDC := extern GDI32.CreateCompatibleDC(dc);

        theHandle := newDC
    }
    
    setTextColor(int color)
    {
        extern GDI32.SetTextColor(theHandle, color)
    }        

    setBkColor(int color)
    {
        extern GDI32.SetBkColor(theHandle, color)
    }        
                        
    select(HBITMAP bitmapHandle)
    {
        extern GDI32.SelectObject(theHandle, bitmapHandle)
    }        
    
    select(HPEN penHandle)
    {
        extern GDI32.SelectObject(theHandle, penHandle)
    }
    
    select(HBRUSH brushHandle)
    {
        extern GDI32.SelectObject(theHandle, brushHandle)
    }        
    
    copy(int destX, int destY, int width, int height, HDC dc, int sourX, int sourY, int flags)
    {
        extern GDI32.BitBlt(theHandle, destX, destY, width, height, dc, sourX, sourY, flags)
        
        //#var(int) err := system'external'KERNEL32 GetLastError.
    }        

    moveTo(int x, int y)
    {
        extern GDI32.MoveToEx(theHandle, x, y, 0)
    }

    lineTo(int x, int y)
    {
        extern GDI32.LineTo(theHandle, x, y)
    }

    drawText(wide text, int x, int y)
    {
        int length := text.Length;
        
        extern GDI32.TextOutW(theHandle, x, y, text, length)
    }
        
    free()
    {
        extern GDI32.DeleteDC(theHandle)
    }
}

// --- HBITMAP ---

public sealed const struct HBITMAP
{
    embeddable __raw theHandle[4];

    embeddable constructor create(HDC dc, int width, int height)
    {
        int h := extern GDI32.CreateCompatibleBitmap(dc, width, height);
        
        theHandle := h
    }

    embeddable constructor load(wide path, int width, int height)
    {
        int h := extern USER32.LoadImageW(
            0,
            path,
            IMAGE_BITMAP,
            width, height,
            LR_LOADFROMFILE);
        
        theHandle := h
    }                
        
    select(HDC handle)
    {
        int retVal := extern GDI32.SelectObject(handle, theHandle)
    }
    
    free()
    {
        extern GDI32.DeleteObject(theHandle)
    }
}

// --- HPEN ---

public sealed const struct HPEN
{
    embeddable __raw theHandle[4];
    
    static HPEN Default
        = 0;

    embeddable constructor(int handle)
    {
        theHandle := handle
    }

    bool IsEmpty()
        = 0 == theHandle;
                        
    embeddable constructor create(int style, int width, int color)
    {
        int h := extern GDI32.CreatePen(style, width, color);
        
        theHandle := h
    }
        
    free()
    {
        extern GDI32.DeleteObject(theHandle)
    }
}

// --- HBRUSH ---

public sealed const struct HBRUSH
{
    embeddable __raw theHandle[4];

    static HBRUSH Default
        = 0;

    embeddable constructor(int handle)
    {
        theHandle := handle
    }

    constructor createSolid(int color)
    {
        int h := extern GDI32.CreateSolidBrush(color);
        
        theHandle := h
    }
        
    free()
    {
        extern GDI32.DeleteObject(theHandle)
    }
}
