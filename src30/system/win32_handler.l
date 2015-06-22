#subject(class:system'COORD)      win_coord.
#subject(class:system'SMALL_RECT) win_small_rect.
#subject(class:system'RECT)       win_rect.

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
    
    #method coord = $self.
    
    #method write : anObject
        <= write &coord:(anObject coord).
    
    #method write &coord:aValue
    [
        #var(type:short)tmp.
        
        aValue readX &vshort:tmp.
        theX := tmp.
        
        aValue readY &vshort:tmp.
        theY := tmp.
    ]
    
    #method readX &vshort:aRetVal
    [
        aRetVal << theX.
    ]
    
    #method readY &vshort:aRetVal
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
    
    #method small_rect = $self.
    
    #method readLeft &vshort:aRetVal
    [
        aRetVal << theLeft.
    ]
    
    #method readTop &vshort:aRetVal
    [
        aRetVal << theTop.
    ]
    
    #method readRight &vshort:aRetVal
    [
        aRetVal << theRight.
    ]
    
    #method readBottom &vshort:aRetVal
    [
        aRetVal << theBottom.
    ]
    
    #method write : anObject
        <= write &small_rect:(anObject small_rect).
    
    #method write &small_rect:aValue
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
