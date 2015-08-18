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
    #field(type:handle)theHandle.

    #constructor(stacksafe) new &hdc:aHandle
        => system'internal'int_copy.
        
    #constructor(stacksafe) newCompatible &hdc:aCompatible
    [
        #var(type:handle)aNewDC := system'external'GDI32 CreateCompatibleDC &handle:aCompatible.

        theHandle := aNewDC.
    ]
    
    #method(stacksafe) setTextColor &int:aColor
    [
        system'external'GDI32 SetTextColor &handle:theHandle &int:aColor.
    ]        

    #method(stacksafe) setBkColor &int:aColor
    [
        system'external'GDI32 SetBkColor &handle:theHandle &int:aColor.
    ]        
                        
    #method(stacksafe) select &hbitmap:aHandle
    [
        system'external'GDI32 SelectObject &handle:theHandle &handle:aHandle.
    ]        
    
    #method(stacksafe) select &hpen:aHandle
    [
        system'external'GDI32 SelectObject &handle:theHandle &hpen:aHandle.
    ]        
    
    #method(stacksafe) select &hbrush:aHandle
    [
        system'external'GDI32 SelectObject &handle:theHandle &handle:aHandle.
    ]        
    
    #method(stacksafe) copy &int:destX &int:destY &int:width &int:height &hdc:dc &int:sourX &int:sourY &int:Flags
    [
        system'external'GDI32 BitBlt &handle:theHandle &int:destX &int:destY &int:width &int:height &handle:dc &int:sourX &int:sourY &int:Flags.
        
        #var(type:int) err := system'external'KERNEL32 GetLastError.
    ]        

    #method(stacksafe) moveTo &int:anX &int:anY
    [
        system'external'GDI32 MoveToEx &handle:theHandle &int:anX &int:anY &int:0.
    ]

    #method(stacksafe) lineTo &int:anX &int:anY
    [
        system'external'GDI32 LineTo &handle:theHandle &int:anX &int:anY.
    ]

    #method(stacksafe) drawText &wide:aText &int:anX &int:anY
    [
        #var(type:vint)aLength.
        aText readLength &vint:aLength.
        
        system'external'GDI32 TextOutW &handle:theHandle &int:anX &int:anY &wide:aText &int:aLength.
    ]
        
    #method(stacksafe) free
    [
        system'external'GDI32 DeleteDC &hdc:$self.
    ]
}

// --- HBITMAP ---

#class(sealed,struct:embeddable) HBITMAP
{
    #field(type:handle)theHandle.

    #constructor(stacksafe) new &hbitmap:aHandle
        => system'internal'int_copy.

    #constructor(stacksafe) new &hdc:aDC &int:aWidth &int:aHeight
    [
        #var(type:handle)h := system'external'GDI32 CreateCompatibleBitmap &handle:aDC &int:aWidth &int:aHeight.
        
        theHandle := h.
    ]

    #constructor(stacksafe) load &wide:aPath &int:aWidth &int:aHeight
    [
        #var(type:handle)h := system'external'USER32 LoadImageW
            &int:0
            &wide:aPath
            &int:IMAGE_BITMAP
            &int:aWidth &int:aHeight
            &int:LR_LOADFROMFILE.
        
        theHandle := h.
    ]                
        
    #method select &hdc:aHandle
    [
        #var(type:int)aRetVal := system'external'GDI32 SelectObject &handle:aHandle &handle:theHandle.
    ]        
    
    #method free
    [
        system'external'GDI32 DeleteObject &handle:theHandle.
    ]
}

// --- HPEN ---

#class(sealed,integer:4) HPEN
{
    #constructor new &hpen:aHandle
        => system'internal'int_copy.
    
    #constructor new &int:aStyle &int:aWidth &int:aColor
    [
        #var(type:hpen)h := system'external'GDI32 CreatePen &int:aStyle &int:aWidth &int:aColor.
        
        $self $save &hpen:h.
    ]
    
    #method(stacksafe) $save &hpen:aHandle
        => system'internal'int_copy.
        
    #method hpen = $self.
            
    #method free
    [
        system'external'GDI32 DeleteObject &hpen:$self.
    ]
}

// --- HBRUSH ---

#class(sealed,struct:embeddable) HBRUSH
{
    #field(type:handle)theHandle.

    #constructor new &hbrush:aHandle
        => system'internal'int_copy.
    
    #constructor new &int:aColor
    [
        #var(type:handle)h := system'external'GDI32 CreateSolidBrush &int:aColor.
        
        theHandle := h.
    ]

    #method hbrush = $self.
        
    #method(stacksafe) free
    [
        system'external'GDI32 DeleteObject &handle:theHandle.
    ]
}
