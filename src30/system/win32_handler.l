#subject(class:system'COORD)coord.
#subject(class:system'SMALL_RECT)small_rect.

// --- Handler ---

#class(sealed,integer:4) Handle
{
    #constructor new &handle:aHandle
        => system'internal'int_copy.

    #constructor min
        => system'internal'int_init.

    #method(type:bool) equal &handle:anOperand 
        => system'internal'int_equal.

    #method handle = $self.
}

// --- RTPointer ---

#class(sealed,struct:embeddable) RTPointer
{
    #field(type:int)theIndex.
    
    #constructor new : anObject
        => system'internal'api_alloc_index.

    #method rt_ptr = $self.            
                        
    #method index
    [
        ^ IntNumber new &int:theIndex.
    ]
    
    #method readIndex &vint:aRetVal
    [
        aRetVal << theIndex.
    ]
        
    #method get
        => system'internal'ptr_get.

    #method free
        => system'internal'api_free_index.
}

// --- UnsafePointer ---

#class(sealed,struct:embeddable) UnsafePointer
{
    #field(type:int)theIndex.

    #constructor new : anObject
    [
        $self write:anObject.
    ]
    
    #method dirty_ptr = $self.
    
    #method write : anObject
        => system'internal'dptr_copy.
                
    #method append &int:aValue
    [
        theIndex := theIndex + aValue.
    ]                            
                            
    #method write &handle:anObject
        => system'internal'int_copy.
    
    #method write &literal:aLiteral
        => system'internal'dptr_copy.
    
    #method write &wide:aLiteral
        => system'internal'dptr_copy.
    
    #method write &shortarray:anArray
        => system'internal'dptr_copy.
    
    #method write &bytearray:anArray
        => system'internal'dptr_copy.
}
                
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