// --- Handler ---

#class(sealed,integer:4) Handle
{
    #constructor(stacksafe) new &handle:aHandle
        => system'internal'int_copy.

    #constructor(stacksafe) new &int:aHandle
        => system'internal'int_copy.

    #constructor min
        => system'internal'int_init.

    #method(type:bool,stacksafe) equal &handle:anOperand 
        => system'internal'int_equal.

    #method handle = $self.
    
    /// Dispatches the verb to the target object with handle subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &handle:$self.
    
    /// Dispatches the verb to the target object with bytearray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &handle:$self.
}

// --- RTPointer ---

#static rt_sync = Object new.

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
    
    #method(stacksafe) readIndex &vint:aRetVal
    [
        aRetVal << theIndex.
    ]
        
    #method(stacksafe) get
        => system'internal'ptr_get.

    #method(stacksafe) free
        => system'internal'api_free_index.
}

// --- UnsafePointer ---

#class(sealed,struct:embeddable) UnsafePointer
{
    #field(type:int)thePointer.

    #constructor new : anObject
    [
        $self write:anObject.
    ]
    
    #method dirty_ptr = $self.
    
    #method(stacksafe) write : anObject
        => system'internal'dptr_copy.
                
    #method(stacksafe) append &int:aValue
    [
        thePointer := thePointer + aValue.
    ]                            
                            
    #method(stacksafe) write &int:anObject
        => system'internal'int_copy.
                
    #method(stacksafe) write &handle:anObject
        => system'internal'int_copy.
    
    #method(stacksafe) write &literal:aLiteral
        => system'internal'dptr_copy.
    
    #method(stacksafe) write &wide:aLiteral
        => system'internal'dptr_copy.
    
    #method(stacksafe) write &shortarray:anArray
        => system'internal'dptr_copy.
    
    #method(stacksafe) write &bytearray:anArray
        => system'internal'dptr_copy.
        
    #method(stacksafe) write &dirty_ptr:aPointer
    [
        #var(type:int)tmp.
        aPointer read &vint:tmp.
        
        thePointer := tmp.
    ]            

    #method(stacksafe) read &vint:aPointer
        => system'internal'dptr_readint.

    #method(stacksafe) read &bytearray:arr &int:len
        => system'internal'dptr_readbytearray.

    // unsafe code
    #method(stacksafe) int
    [
        #var(type:int)aValue.
        $self read &vint:aValue.
        
        ^ IntNumber new &int:aValue.
    ]

    // unsafe code
    #method(stacksafe) literal
    [
        #var(type:int)aLen := system'external'KERNEL32 lstrlenA &int:thePointer.
        #var(type:literal)aLiteral := LiteralValue new &int:aLen.
        system'external'kernel32 lstrcpyA &literal:aLiteral &int:thePointer.
        
        ^ aLiteral.
    ]        

    // unsafe code
    #method(stacksafe) wide
    [
        #var(type:int)aLen := system'external'KERNEL32 lstrlenW &int:thePointer.
        #var(type:wide)aLiteral := WideLiteralValue new &int:aLen.
        system'external'kernel32 lstrcpyW &wide:aLiteral &int:thePointer.
        
        ^ aLiteral.
    ]
    
    #method(stacksafe,type:bool)is &empty
        = 0 == thePointer.
    
    /// Dispatches the verb to the target object with handle subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &dirty_ptr:$self.
    
    /// Dispatches the verb to the target object with bytearray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &dirty_ptr:$self.    
}

// --- COORD ---

#subject(class:system'COORD)win_coord.

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
    
    #method(stacksafe) write &short:X &short:Y
    [
        theX := X.
        theY := Y.
    ]
    
    #method(stacksafe) write &win_coord:aValue
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

#subject(class:system'SMALL_RECT) win_small_rect.
                
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
