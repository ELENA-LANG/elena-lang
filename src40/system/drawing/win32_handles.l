// --- Raster Operation Codes ---

public const int SRCCOPY          = 00CC0020h;

//const int IMAGE_BITMAP     = 0.
//
//const int LR_LOADFROMFILE  = 0010h.

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
        int get() = theLeft;
        
        set(int v)
        {
            theLeft := v
        }
    }
    
    int Top
    {
        int get() = theTop;
        
        set(int v)
        {
            theTop := v
        }
    }
    
    int Right
    {
        int get() = theRight;
        
        set(int v)
        {
            theRight := v
        }
    }
    
    int Bottom
    {
        int get() = theBottom;
        
        set(int v)
        {
            theBottom := v
        }
    }

    int Width
    {
        int get()
        {
            int x2 := theRight;
            int x1 := theLeft;
            int w := x2 - x1;
            
            ^ w
        }
    }

    int Height
    {
        int get()
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

//    stacksafe constructor new hdc:aHandle
//    [
//        theHandle := aHandle.
//    ]
//
//    stacksafe constructor newCompatible hdc:aCompatible
//    [
//        int aNewDC := system'external'GDI32 CreateCompatibleDC int:aCompatible.
//
//        theHandle := aNewDC.
//    ]
//    
//    stacksafe setTextColor int:aColor
//    [
//        system'external'GDI32 SetTextColor int:theHandle int:aColor.
//    ]        
//
//    stacksafe setBkColor int:aColor
//    [
//        system'external'GDI32 SetBkColor int:theHandle int:aColor.
//    ]        
//                        
//    stacksafe select hbitmap:aBitmapHandle
//    [
//        system'external'GDI32 SelectObject int:theHandle int:aBitmapHandle.
//    ]        
//    
//    stacksafe select hpen:aPenHandle
//    [
//        system'external'GDI32 SelectObject int:theHandle int:aPenHandle.
//    ]        
//    
//    stacksafe select hbrush:aBrushHandle
//    [
//        system'external'GDI32 SelectObject int:theHandle int:aBrushHandle.
//    ]        
    
    copy(int destX, int destY, int width, int height, HDC dc, int sourX, int sourY, int flags)
    {
        extern GDI32.BitBlt(theHandle, destX, destY, width, height, dc, sourX, sourY, flags)
        
        //#var(int) err := system'external'KERNEL32 GetLastError.
    }        

//    stacksafe moveTo int:anX int:anY
//    [
//        system'external'GDI32 MoveToEx int:theHandle int:anX int:anY int:0.
//    ]
//
//    stacksafe lineTo int:anX int:anY
//    [
//        system'external'GDI32 LineTo int:theHandle int:anX int:anY.
//    ]
//
//    stacksafe drawText wide:aText int:anX int:anY
//    [
//        int aLength := aText length.
//        
//        system'external'GDI32 TextOutW int:theHandle int:anX int:anY wide:aText int:aLength.
//    ]
        
    free()
    {
        extern GDI32.DeleteDC(theHandle)
    }
}

// --- HBITMAP ---

public sealed const struct HBITMAP
{
    embeddable __raw theHandle[4];

//    stacksafe constructor new hbitmap:aHandle
//    [
//        theHandle := aHandle.
//    ]
//
//    stacksafe constructor new hdc:aDC int:aWidth int:aHeight
//    [
//        int h := system'external'GDI32 CreateCompatibleBitmap int:aDC int:aWidth int:aHeight.
//        
//        theHandle := h.
//    ]
//
//    stacksafe constructor load wide:aPath int:aWidth int:aHeight
//    [
//        int h := system'external'USER32 LoadImageW
//            int:0
//            wide:aPath
//            int:IMAGE_BITMAP
//            int:aWidth int:aHeight
//            int:LR_LOADFROMFILE.
//        
//        theHandle := h
//    ]                
        
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
                        
//    stacksafe constructor new hpen:aHandle
//    [
//        theHandle := aHandle.
//    ]
//    
//    stacksafe constructor new int:aStyle int:aWidth int:aColor
//    [
//        int h := system'external'GDI32 CreatePen int:aStyle int:aWidth int:aColor.
//        
//        theHandle := h.
//    ]
        
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

//    constructor new hbrush:aHandle
//    [
//        theHandle := aHandle.
//    ]
//    
//    constructor new int:aColor
//    [
//        int h := system'external'GDI32 CreateSolidBrush int:aColor.
//        
//        theHandle := h.
//    ]
        
    free()
    {
        extern GDI32.DeleteObject(theHandle)
    }
}
