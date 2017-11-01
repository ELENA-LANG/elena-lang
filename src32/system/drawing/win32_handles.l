// data structure wrappers
type hdc      = system'drawing'HDC.
type hbitmap  = system'drawing'HBITMAP.
type hpen     = system'drawing'HPEN.
type hbrush   = system'drawing'HBRUSH.

type win_rect = system'drawing'RECT.

// --- Raster Operation Codes ---

const int SRCCOPY          = 00CC0020h.

const int IMAGE_BITMAP     = 0.

const int LR_LOADFROMFILE  = 0010h.

// --- RECT ---

sealed struct RECT
{
    int       theLeft.
    int       theTop.
    int       theRight.
    int       theBottom.
    
    embeddable constructor new
    [
        theLeft := 0.
        theTop := 0.
        theRight := 0.
        theBottom := 0.
    ]
    
    int vprop left   :: readLeftTo, setLeft.
    int vprop top    :: readTopTo, setTop.
    int vprop right  :: readRightTo, setRight.
    int vprop bottom :: readBottomTo, setBottom.
    int rvprop width  :: readWidthTo.
    int rvprop height :: readHeightTo.
    
    stacksafe setLeft int:aValue
    [
        theLeft := aValue.
    ]
    
    stacksafe setTop int:aValue
    [
        theTop := aValue.
    ]
    
    stacksafe setRight int:aValue
    [
        theRight := aValue.
    ]
    
    stacksafe setBottom int:aValue
    [
        theBottom := aValue.
    ]
    
    stacksafe readLeftTo vint:aRetVal
    [
        aRetVal int := theLeft.
    ]
        
    stacksafe readTopTo vint:aRetVal
    [
        aRetVal int := theTop.
    ]
    
    stacksafe readRightTo vint:aRetVal
    [
        aRetVal int := theRight.
    ]
    
    stacksafe readBottomTo vint:aRetVal
    [
        aRetVal int := theBottom.
    ]
    
    stacksafe readWidthTo vint:aRetVal
    [
        int x2 := theRight.
        int x1 := theLeft.
        int w := x2 - x1.
        
        aRetVal int := w.
    ]
    
    stacksafe readHeightTo vint:aRetVal
    [
        int y2 := theBottom.
        int y1 := theTop.
        int h := y2 - y1.
        
        aRetVal int := h.
    ]
}

// --- HDC ---

sealed const struct HDC
{
    __dword& theHandle(4).

    stacksafe implicit int:handle
    [
        theHandle := handle.
    ]

    stacksafe constructor new hdc:aHandle
    [
        theHandle := aHandle.
    ]

    stacksafe constructor newCompatible hdc:aCompatible
    [
        int aNewDC := system'external'GDI32 CreateCompatibleDC int:aCompatible.

        theHandle := aNewDC.
    ]
    
    stacksafe setTextColor int:aColor
    [
        system'external'GDI32 SetTextColor int:theHandle int:aColor.
    ]        

    stacksafe setBkColor int:aColor
    [
        system'external'GDI32 SetBkColor int:theHandle int:aColor.
    ]        
                        
    stacksafe select hbitmap:aBitmapHandle
    [
        system'external'GDI32 SelectObject int:theHandle int:aBitmapHandle.
    ]        
    
    stacksafe select hpen:aPenHandle
    [
        system'external'GDI32 SelectObject int:theHandle int:aPenHandle.
    ]        
    
    stacksafe select hbrush:aBrushHandle
    [
        system'external'GDI32 SelectObject int:theHandle int:aBrushHandle.
    ]        
    
    stacksafe copy int:destX int:destY int:width int:height hdc:dc int:sourX int:sourY int:Flags
    [
        system'external'GDI32 BitBlt int:theHandle int:destX int:destY int:width int:height int:dc int:sourX int:sourY int:Flags.
        
        //#var(int) err := system'external'KERNEL32 GetLastError.
    ]        

    stacksafe moveTo int:anX int:anY
    [
        system'external'GDI32 MoveToEx int:theHandle int:anX int:anY int:0.
    ]

    stacksafe lineTo int:anX int:anY
    [
        system'external'GDI32 LineTo int:theHandle int:anX int:anY.
    ]

    stacksafe drawText wide:aText int:anX int:anY
    [
        int aLength := aText length.
        
        system'external'GDI32 TextOutW int:theHandle int:anX int:anY wide:aText int:aLength.
    ]
        
    stacksafe free
    [
        system'external'GDI32 DeleteDC int:theHandle.
    ]
}

// --- HBITMAP ---

sealed const struct HBITMAP
{
    __dword& theHandle(4).

    stacksafe constructor new hbitmap:aHandle
    [
        theHandle := aHandle.
    ]

    stacksafe constructor new hdc:aDC int:aWidth int:aHeight
    [
        int h := system'external'GDI32 CreateCompatibleBitmap int:aDC int:aWidth int:aHeight.
        
        theHandle := h.
    ]

    stacksafe constructor load wide:aPath int:aWidth int:aHeight
    [
        int h := system'external'USER32 LoadImageW
            int:0
            wide:aPath
            int:IMAGE_BITMAP
            int:aWidth int:aHeight
            int:LR_LOADFROMFILE.
        
        theHandle := h
    ]                
        
    select hdc:aHandle
    [
        int aRetVal := system'external'GDI32 SelectObject int:aHandle int:theHandle.
    ]        
    
    free
    [
        system'external'GDI32 DeleteObject int:theHandle.
    ]
}

// --- HPEN ---

sealed const struct HPEN
{
    __dword& theHandle(4).
    
    stacksafe constructor new hpen:aHandle
    [
        theHandle := aHandle.
    ]
    
    stacksafe constructor new int:aStyle int:aWidth int:aColor
    [
        int h := system'external'GDI32 CreatePen int:aStyle int:aWidth int:aColor.
        
        theHandle := h.
    ]
        
    stacksafe free
    [
        system'external'GDI32 DeleteObject int:theHandle.
    ]
}

// --- HBRUSH ---

sealed const struct HBRUSH
{
    __dword& theHandle(4).

    constructor new hbrush:aHandle
    [
        theHandle := aHandle.
    ]
    
    constructor new int:aColor
    [
        int h := system'external'GDI32 CreateSolidBrush int:aColor.
        
        theHandle := h.
    ]
        
    stacksafe free
    [
        system'external'GDI32 DeleteObject int:theHandle.
    ]
}

type vhdc     = ref<HDC>.
