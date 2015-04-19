#define system.

// --- HInstance ---

#class(sealed,integer:4) HInstance
{
    #constructor new &hinstance:aHandle
        => system'internal'int_copy.
        
    #constructor current
    //    <= new &hinstance:(system'external'KERNEL32 GetModuleHandleW &int:0).
    [
        #var(type:hinstance)aCurrent := system'external'KERNEL32 GetModuleHandleW &int:0.
        
        $self $write &hinstance:aCurrent.
    ]

    #method $write &hinstance:anInstance
        => system'internal'int_copy.

    #method $read &vint:aRetVal
        => system'internal'int_save.

    #method hinstance = $self.
}

// --- MSG ---

#class(sealed,struct:embeddable) MSG 
{
    #field(type:handle)theHWND.
    #field(type:int)   theMessage.
    #field(type:int)   theWParam.
    #field(type:int)   theLParam.
    #field(type:int)   theTime.
    #field(type:int)   theX.
    #field(type:int)   theY.    
    
    #method win_msg = $self.
}

// --- CREATESTRUCT ---

#class(sealed,struct:embeddable)CREATESTRUCT
{
    #field(type:int)       theCreateParam.
    #field(type:hinstance) theInstance.    
    #field(type:int)       theMenu.
    #field(type:int)       theParent.
    #field(type:int)       theHeight.
    #field(type:int)       theWidth.
    #field(type:int)       theY.
    #field(type:int)       theX.
    #field(type:int)       theStyle.
    #field(type:int)       theNamePtr.
    #field(type:int)       theClassPtr.
    #field(type:int)       theExStyle.
    
    #method readCreateParam &vint:aRetVal
        => system'internal'int_save.
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

#class(sealed,integer:4) HDC
{
    #constructor new &hdc:aHandle
        => system'internal'int_copy.
        
    #constructor newCompatible &hdc:aCompatible
    [
        #var(type:hdc)aNewDC := system'external'GDI32 CreateCompatibleDC &hdc:aCompatible.
        
        $self $save &hdc:aNewDC.
    ]
    
    #method $save &hdc:aHandle
        => system'internal'int_copy.

    #method setTextColor &int:aColor
    [
        system'external'GDI32 SetTextColor &hdc:$self &int:aColor.
    ]        

    #method setBkColor &int:aColor
    [
        system'external'GDI32 SetBkColor &hdc:$self &int:aColor.
    ]        
                        
    #method select &hbitmap:aHandle
    [
        system'external'GDI32 SelectObject &hdc:$self &hbitmap:aHandle.
    ]        
    
    #method select &hpen:aHandle
    [
        system'external'GDI32 SelectObject &hdc:$self &hpen:aHandle.
    ]        
    
    #method select &hbrush:aHandle
    [
        system'external'GDI32 SelectObject &hdc:$self &hbrush:aHandle.
    ]        
    
    #method copy &int:destX &int:destY &int:width &int:height &hdc:dc &int:sourX &int:sourY &int:Flags
    [
        system'external'GDI32 BitBlt &hdc:$self &int:destX &int:destY &int:width &int:height &hdc:dc &int:sourX &int:sourY &int:Flags.
    ]        

    #method moveTo &int:anX &int:anY
    [
        system'external'GDI32 MoveToEx &hdc:$self &int:anX &int:anY &int:0.
    ]

    #method lineTo &int:anX &int:anY
    [
        system'external'GDI32 LineTo &hdc:$self &int:anX &int:anY.
    ]

    #method drawText &wide:aText &int:anX &int:anY
    [
        #var(type:vint)aLength.
        aText readLength &vint:aLength.
        
        system'external'GDI32 TextOutW &hdc:$self &int:anX &int:anY &wide:aText &int:aLength.
    ]
        
    #method free
    [
        system'external'GDI32 DeleteDC &hdc:$self.
    ]
}

// --- HBITMAP ---

#class(sealed,integer:4) HBITMAP
{
    #constructor new &hbitmap:aHandle
        => system'internal'int_copy.

    #constructor new &hdc:aDC &int:aWidth &int:aHeight
    [
        #var(type:hbitmap)h := system'external'GDI32 CreateCompatibleBitmap &hdc:aDC &int:aWidth &int:aHeight.
        
        $self $save &hbitmap:h.
    ]
                
    #method $save &hbitmap:aHandle
        => system'internal'int_copy.
        
    #method select &hdc:aHandle
    [
        system'external'GDI32 SelectObject &hdc:aHandle &hbitmap:$self.
    ]        
    
    #method free
    [
        system'external'GDI32 DeleteObject &hbitmap:$self.
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
    
    #method $save &hpen:aHandle
        => system'internal'int_copy.
        
    #method hpen = $self.
            
    #method free
    [
        system'external'GDI32 DeleteObject &hpen:$self.
    ]
}

// --- HBRUSH ---

#class(sealed,integer:4) HBRUSH
{
    #constructor new &hbrush:aHandle
        => system'internal'int_copy.
    
    #constructor new &int:aColor
    [
        #var(type:hbrush)h := system'external'GDI32 CreateSolidBrush &int:aColor.
        
        $self $save &hbrush:h.
    ]

    #method hbrush = $self.
            
    #method $save &hbrush:aHandle
        => system'internal'int_copy.
        
    #method free
    [
        system'external'GDI32 DeleteObject &hbrush:$self.
    ]
}
