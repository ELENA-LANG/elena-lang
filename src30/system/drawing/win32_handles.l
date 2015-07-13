#define system.

// data structure wrappers
#subject(class:system'drawing'HDC)         hdc.
#subject(class:system'drawing'HBITMAP)     hbitmap.
#subject(class:system'drawing'HPEN)        hpen.
#subject(class:system'drawing'HBRUSH)      hbrush.

#subject(class:system'drawing'COORD)       win_coord.
#subject(class:system'drawing'SMALL_RECT)  win_small_rect.
#subject(class:system'drawing'RECT)        win_rect.

// --- Raster Operation Codes ---

#symbol(const,type:int)SRCCOPY          = 00CC0020h.

#symbol(const,type:int)IMAGE_BITMAP     = 0.

#symbol(const,type:int)LR_LOADFROMFILE  = 0010h.

// --- COORD --

#class(sealed,struct:embeddable) COORD
{
    #field(type:short)theX.
    #field(type:short)theY.
    
    #constructor new
    [
        theX := 0.
        theY := 0.
    ]
    
    #method win_coord = $self.
    
    #method write : anObject
        <= write &win_coord:(anObject win_coord).
    
    #method write &win_coord:aValue
    [
        #var(type:short)tmp.
        
        aValue readX &vshort:tmp.
        theX := tmp.
        
        aValue readY &vshort:tmp.
        theY := tmp.
    ]
    
    #method(stacksafe) readX &vshort:aRetVal
    [
        aRetVal << theX.
    ]
    
    #method(stacksafe) readY &vshort:aRetVal
    [
        aRetVal << theY.
    ]
    
    #method x
    [
        #var(type:short)anX.
        $self readX &vshort:anX.
        
        ^ ShortNumber new &short:anX.
    ]
    
    #method y
    [
        #var(type:short)anY.
        $self readY &vshort:anY.
        
        ^ ShortNumber new &short:anY.
    ]
}
                
// --- SMALL_RECT --

#class(sealed,struct:embeddable) SMALL_RECT
{
    #field(type:short)theLeft.
    #field(type:short)theTop.
    #field(type:short)theRight.
    #field(type:short)theBottom.
    
    #method win_small_rect = $self.
    
    #method(stacksafe) readLeft &vshort:aRetVal
    [
        aRetVal << theLeft.
    ]
    
    #method(stacksafe) readTop &vshort:aRetVal
    [
        aRetVal << theTop.
    ]
    
    #method(stacksafe) readRight &vshort:aRetVal
    [
        aRetVal << theRight.
    ]
    
    #method(stacksafe) readBottom &vshort:aRetVal
    [
        aRetVal << theBottom.
    ]
    
    #method write : anObject
        <= write &win_small_rect:(anObject win_small_rect).
    
    #method write &win_small_rect:aValue
    [
        #var(type:short)tmp.
        
        aValue readLeft &vshort:tmp.
        theLeft := tmp.
        
        aValue readTop &vshort:tmp.
        theTop := tmp.
        
        aValue readRight &vshort:tmp.
        theRight := tmp.
        
        aValue readBottom &vshort:tmp.
        theBottom := tmp.
    ]
    
    #method left
    [
        #var(type:short)aLeft.
        $self readLeft &vshort:aLeft.
        
        ^ ShortNumber new &short:aLeft.
    ]
    
    #method top
    [
        #var(type:short)aTop.
        $self readTop &vshort:aTop.
        
        ^ ShortNumber new &short:aTop.
    ]
    
    #method right
    [
        #var(type:short)aRight.
        $self readRight &vshort:aRight.
        
        ^ ShortNumber new &short:aRight.
    ]
    
    #method bottom
    [
        #var(type:short)aBottom.
        $self readBottom &vshort:aBottom.
        
        ^ ShortNumber new &short:aBottom.
    ]
}    

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

    #constructor new &hbitmap:aHandle
        => system'internal'int_copy.

    #constructor new &hdc:aDC &int:aWidth &int:aHeight
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
        system'external'GDI32 SelectObject &handle:aHandle &handle:theHandle.
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
        
    #method free
    [
        system'external'GDI32 DeleteObject &handle:theHandle.
    ]
}
