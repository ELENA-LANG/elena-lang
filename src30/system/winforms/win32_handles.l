#define system.

// --- HInstance ---

#class(sealed,struct:embeddable) HInstance
{
    #field(type:handle)theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]
        
    #constructor current
    //    <= new &hinstance:(system'external'KERNEL32 GetModuleHandleW &int:0).
    [
        #var(type:handle)aCurrent := system'external'KERNEL32 GetModuleHandleW &int:0.
        
        theHandle := aCurrent.
    ]

    #method hinstance = $self.
}

// --- HINSTANCE ---

#static(type:hinstance) CurrentInstance = HInstance current.

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

#class(sealed,struct:embeddable) HDC
{
    #field(type:handle)theHandle.

    #constructor new &hdc:aHandle
        => system'internal'int_copy.
        
    #constructor newCompatible &hdc:aCompatible
    [
        #var(type:handle)aNewDC := system'external'GDI32 CreateCompatibleDC &handle:aCompatible.

        theHandle := aNewDC.
    ]
    
    #method setTextColor &int:aColor
    [
        system'external'GDI32 SetTextColor &handle:theHandle &int:aColor.
    ]        

    #method setBkColor &int:aColor
    [
        system'external'GDI32 SetBkColor &handle:theHandle &int:aColor.
    ]        
                        
    #method select &hbitmap:aHandle
    [
        system'external'GDI32 SelectObject &handle:theHandle &handle:aHandle.
    ]        
    
    #method select &hpen:aHandle
    [
        system'external'GDI32 SelectObject &handle:theHandle &hpen:aHandle.
    ]        
    
    #method select &hbrush:aHandle
    [
        system'external'GDI32 SelectObject &handle:theHandle &handle:aHandle.
    ]        
    
    #method copy &int:destX &int:destY &int:width &int:height &hdc:dc &int:sourX &int:sourY &int:Flags
    [
        system'external'GDI32 BitBlt &handle:theHandle &int:destX &int:destY &int:width &int:height &handle:dc &int:sourX &int:sourY &int:Flags.
    ]        

    #method moveTo &int:anX &int:anY
    [
        system'external'GDI32 MoveToEx &handle:theHandle &int:anX &int:anY &int:0.
    ]

    #method lineTo &int:anX &int:anY
    [
        system'external'GDI32 LineTo &handle:theHandle &int:anX &int:anY.
    ]

    #method drawText &wide:aText &int:anX &int:anY
    [
        #var(type:vint)aLength.
        aText readLength &vint:aLength.
        
        system'external'GDI32 TextOutW &handle:theHandle &int:anX &int:anY &wide:aText &int:aLength.
    ]
        
    #method free
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

    #constructor load &wide:aPath &int:aWidth &int:aHeight
    [
        #var(type:handle)h := system'external'USER32 LoadImageW
            &handle:CurrentInstance
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
    
    #method $save &hpen:aHandle
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

// --- HMENU ---

#class(sealed,struct:embeddable) HMENU
{
    #field(type:handle)theHandle.
    
    #constructor new
    [
        #var(type:handle)h := system'external'USER32 CreateMenu.
        
        theHandle := h.
    ]

    #constructor newPopup
    [
        #var(type:handle)h := system'external'USER32 CreatePopupMenu.
        
        theHandle := h.
    ]

    #method append &int:Flags &int:anID &wide:aCaption
    [
        system'external'USER32 AppendMenuW &handle:theHandle &int:Flags &int:anID &wide:aCaption.
    ]
            
    #method append &int:Flags &handle:aParent &wide:aCaption
    [
        system'external'USER32 AppendMenuW &handle:theHandle &int:(Flags || MF_POPUP) &handle:aParent &wide:aCaption.
    ]
            
    #method free
    [
        system'external'USER32 DestroyMenu &handle:theHandle.
    ]
}