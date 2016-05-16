#import system.

// data structure wrappers
#subject hdc      = system'drawing'HDC.
#subject hbitmap  = system'drawing'HBITMAP.
#subject hpen     = system'drawing'HPEN.
#subject hbrush   = system'drawing'HBRUSH.

#subject vhdc     = varof:hdc.

#subject win_rect = system'drawing'RECT.

// --- Raster Operation Codes ---

#symbol(const,int)SRCCOPY          = 00CC0020h.

#symbol(const,int)IMAGE_BITMAP     = 0.

#symbol(const,int)LR_LOADFROMFILE  = 0010h.

// --- RECT ---

#class(sealed,struct:embeddable)RECT
{
    #field(int)       theLeft.
    #field(int)       theTop.
    #field(int)       theRight.
    #field(int)       theBottom.
    
    #method(stacksafe,innprop:left) setLeft &int:aValue
    [
        theLeft := aValue.
    ]
    
    #method(stacksafe,innprop:top) setTop &int:aValue
    [
        theTop := aValue.
    ]
    
    #method(stacksafe,innprop:right) setRight &int:aValue
    [
        theRight := aValue.
    ]
    
    #method(stacksafe,innprop:bottom) setBottom &int:aValue
    [
        theBottom := aValue.
    ]
    
    #method(stacksafe,outnprop:left) readLeft &vint:aRetVal
    [
        aRetVal := theLeft.
    ]
    
    #method(stacksafe,outnprop:top) readTop &vint:aRetVal
    [
        aRetVal := theTop.
    ]
    
    #method(stacksafe,outnprop:right) readRight &vint:aRetVal
    [
        aRetVal := theRight.
    ]
    
    #method(stacksafe,outnprop:bottom) readBottom &vint:aRetVal
    [
        aRetVal := theBottom.
    ]
}

// --- HDC ---

#class(sealed,struct:embeddable,const,variable) HDC
{
    #field(int)theHandle.

    #constructor(stacksafe) new &hdc:aHandle
    [
        theHandle := aHandle.
    ]

    #constructor(stacksafe) newCompatible &hdc:aCompatible
    [
        #var(int)aNewDC := system'external'GDI32 CreateCompatibleDC &int:aCompatible.

        theHandle := aNewDC.
    ]
    
    #method(stacksafe) setTextColor &int:aColor
    [
        system'external'GDI32 SetTextColor &int:theHandle &int:aColor.
    ]        

    #method(stacksafe) setBkColor &int:aColor
    [
        system'external'GDI32 SetBkColor &int:theHandle &int:aColor.
    ]        
                        
    #method(stacksafe) select &hbitmap:aBitmapHandle
    [
        system'external'GDI32 SelectObject &int:theHandle &int:aBitmapHandle.
    ]        
    
    #method(stacksafe) select &hpen:aPenHandle
    [
        system'external'GDI32 SelectObject &int:theHandle &int:aPenHandle.
    ]        
    
    #method(stacksafe) select &hbrush:aBrushHandle
    [
        system'external'GDI32 SelectObject &int:theHandle &int:aBrushHandle.
    ]        
    
    #method(stacksafe) copy &int:destX &int:destY &int:width &int:height &hdc:dc &int:sourX &int:sourY &int:Flags
    [
        system'external'GDI32 BitBlt &int:theHandle &int:destX &int:destY &int:width &int:height &int:dc &int:sourX &int:sourY &int:Flags.
        
        //#var(int) err := system'external'KERNEL32 GetLastError.
    ]        

    #method(stacksafe) moveTo &int:anX &int:anY
    [
        system'external'GDI32 MoveToEx &int:theHandle &int:anX &int:anY &int:0.
    ]

    #method(stacksafe) lineTo &int:anX &int:anY
    [
        system'external'GDI32 LineTo &int:theHandle &int:anX &int:anY.
    ]

    #method(stacksafe) drawText &wide:aText &int:anX &int:anY
    [
        #var(int)aLength := aText length.
        
        system'external'GDI32 TextOutW &int:theHandle &int:anX &int:anY &wide:aText &int:aLength.
    ]
        
    #method(stacksafe) free
    [
        system'external'GDI32 DeleteDC &int:theHandle.
    ]
}

// --- HBITMAP ---

#class(sealed,struct:embeddable,const,variable) HBITMAP
{
    #field(int)theHandle.

    #constructor(stacksafe) new &hbitmap:aHandle
    [
        theHandle := aHandle.
    ]

    #constructor(stacksafe) new &hdc:aDC &int:aWidth &int:aHeight
    [
        #var(int)h := system'external'GDI32 CreateCompatibleBitmap &int:aDC &int:aWidth &int:aHeight.
        
        theHandle := h.
    ]

    #constructor(stacksafe) load &wide:aPath &int:aWidth &int:aHeight
    [
        #var(int)h := system'external'USER32 LoadImageW
            &int:0
            &wide:aPath
            &int:IMAGE_BITMAP
            &int:aWidth &int:aHeight
            &int:LR_LOADFROMFILE.
        
        theHandle := h.
    ]                
        
    #method select &hdc:aHandle
    [
        #var(int)aRetVal := system'external'GDI32 SelectObject &int:aHandle &int:theHandle.
    ]        
    
    #method free
    [
        system'external'GDI32 DeleteObject &int:theHandle.
    ]
}

// --- HPEN ---

#class(sealed,struct:embeddable,const,variable) HPEN
{
    #field(int)theHandle.
    
    #constructor(stacksafe) new &hpen:aHandle
    [
        theHandle := aHandle.
    ]
    
    #constructor(stacksafe) new &int:aStyle &int:aWidth &int:aColor
    [
        #var(int)h := system'external'GDI32 CreatePen &int:aStyle &int:aWidth &int:aColor.
        
        theHandle := h.
    ]
        
    #method(stacksafe) free
    [
        system'external'GDI32 DeleteObject &int:theHandle.
    ]
}

// --- HBRUSH ---

#class(sealed,struct:embeddable,const,variable) HBRUSH
{
    #field(int)theHandle.

    #constructor new &hbrush:aHandle
    [
        theHandle := aHandle.
    ]
    
    #constructor new &int:aColor
    [
        #var(int)h := system'external'GDI32 CreateSolidBrush &int:aColor.
        
        theHandle := h.
    ]
        
    #method(stacksafe) free
    [
        system'external'GDI32 DeleteObject &int:theHandle.
    ]
}
