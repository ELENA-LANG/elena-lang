// ==== ver 3.4.1 ===

#import system.

// data structure wrappers
#subject(class:system'drawing'HDC)         hdc.
#subject(class:system'drawing'HBITMAP)     hbitmap.
#subject(class:system'drawing'HPEN)        hpen.
#subject(class:system'drawing'HBRUSH)      hbrush.

#subject(class:system'drawing'RECT)        win_rect.

// --- Raster Operation Codes ---

#symbol(const,type:int)SRCCOPY          = 00CC0020h.

#symbol(const,type:int)IMAGE_BITMAP     = 0.

#symbol(const,type:int)LR_LOADFROMFILE  = 0010h.

// --- RECT ---

#class(sealed,struct:embeddable)RECT
{
    #field(type:int)       theLeft.
    #field(type:int)       theTop.
    #field(type:int)       theRight.
    #field(type:int)       theBottom.
    
    #method(type:int)left = theLeft.
    
    #method(type:int)top = theTop.
    
    #method(type:int)right = theRight.
    
    #method(type:int)bottom = theBottom.
    
    #method setLeft &int:aValue
    [
        theLeft := aValue.
    ]
    
    #method setTop &int:aValue
    [
        theTop := aValue.
    ]
    
    #method setRight &int:aValue
    [
        theRight := aValue.
    ]
    
    #method setBottom &int:aValue
    [
        theBottom := aValue.
    ]
    
    #method readLeft &vint:aRetVal
    [
        aRetVal << theLeft.
    ]
    
    #method readTop &vint:aRetVal
    [
        aRetVal << theTop.
    ]
    
    #method readRight &vint:aRetVal
    [
        aRetVal << theRight.
    ]
    
    #method readBottom &vint:aRetVal
    [
        aRetVal << theBottom.
    ]
}

// --- HDC ---

#class(sealed,struct:embeddable) HDC
{
    #field(type:int)theHandle.

    #constructor(stacksafe) new &hdc:aHandle
        => system'internal'int_copy.
        
    #constructor(stacksafe) newCompatible &hdc:aCompatible
    [
        #var(type:handle)aNewDC := system'external'GDI32 CreateCompatibleDC &int:aCompatible.

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
        
        #var(type:int) err := system'external'KERNEL32 GetLastError.
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
        #var(type:vint)aLength.
        aText readLength &vint:aLength.
        
        system'external'GDI32 TextOutW &int:theHandle &int:anX &int:anY &wide:aText &int:aLength.
    ]
        
    #method(stacksafe) free
    [
        system'external'GDI32 DeleteDC &int:theHandle.
    ]
}

// --- HBITMAP ---

#class(sealed,struct:embeddable) HBITMAP
{
    #field(type:int)theHandle.

    #constructor(stacksafe) new &hbitmap:aHandle
        => system'internal'int_copy.

    #constructor(stacksafe) new &hdc:aDC &int:aWidth &int:aHeight
    [
        #var(type:handle)h := system'external'GDI32 CreateCompatibleBitmap &int:aDC &int:aWidth &int:aHeight.
        
        theHandle := h.
    ]

    #constructor(stacksafe) load &wide:aPath &int:aWidth &int:aHeight
    [
        #var(type:int)h := system'external'USER32 LoadImageW
            &int:0
            &wide:aPath
            &int:IMAGE_BITMAP
            &int:aWidth &int:aHeight
            &int:LR_LOADFROMFILE.
        
        theHandle := h.
    ]                
        
    #method select &hdc:aHandle
    [
        #var(type:int)aRetVal := system'external'GDI32 SelectObject &int:aHandle &int:theHandle.
    ]        
    
    #method free
    [
        system'external'GDI32 DeleteObject &int:theHandle.
    ]
}

// --- HPEN ---

#class(sealed,struct:embeddable) HPEN
{
    #field(type:int)theHandle.
    
    #constructor(stacksafe) new &hpen:aHandle
        => system'internal'int_copy.
    
    #constructor(stacksafe) new &int:aStyle &int:aWidth &int:aColor
    [
        #var(type:int)h := system'external'GDI32 CreatePen &int:aStyle &int:aWidth &int:aColor.
        
        theHandle := h.
    ]
        
    #method hpen = $self.
            
    #method(stacksafe) free
    [
        system'external'GDI32 DeleteObject &int:theHandle.
    ]
}

// --- HBRUSH ---

#class(sealed,struct:embeddable) HBRUSH
{
    #field(type:int)theHandle.

    #constructor new &hbrush:aHandle
        => system'internal'int_copy.
    
    #constructor new &int:aColor
    [
        #var(type:int)h := system'external'GDI32 CreateSolidBrush &int:aColor.
        
        theHandle := h.
    ]

    #method hbrush = $self.
        
    #method(stacksafe) free
    [
        system'external'GDI32 DeleteObject &int:theHandle.
    ]
}
