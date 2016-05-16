// --- Handler ---

#class(sealed,struct:embeddable,variable,dispatchable:handle,const) Handle
{
    #field(int)theHandle.
    
    #constructor(stacksafe) new &handle:h
    [
        theHandle := h.
    ]

    #constructor(stacksafe) new &int:aHandle
    [
        theHandle := aHandle.
    ]

    #method(bool,stacksafe) equal &handle:anOperand 
        = theHandle == anOperand.
}

// --- RTPointer ---

#static rt_sync = Object new.

#class(sealed,struct:embeddable,variable,const) RTPointer
{
    #field(int)theIndex.
    
    #constructor new : anObject
    [
        system'core_routines'__rtptr new &object:$self &object:anObject.
    ]

    #method(stacksafe,outnprop:index) readIndex &vint:aRetVal
    [
        aRetVal := theIndex.
    ]
        
    #method(stacksafe) get
        => system'core_routines'__rtptr.

    #method(stacksafe)  free
        => system'core_routines'__rtptr.
}

// --- UnsafePointer ---

#class(sealed,struct:embeddable,dispatchable:dirty_ptr) UnsafePointer
{
    #field(int)thePointer.

    #constructor new : anObject
    [
        $self set:anObject.
    ]
    
    #method(stacksafe) set : anObject
        => system'core_routines'__dptr.
                
    #method(stacksafe) set &int:aValue
    [
        thePointer := aValue.
    ]
                
    #method(stacksafe) offset &int:aValue
    [
        thePointer := thePointer + aValue.
    ]                            

    #method(stacksafe) set &dirty_ptr:anObject
        => system'core_routines'__dptr.
                
    #method(stacksafe) write &int:anObject
        => system'core_routines'__dptr.
                
    #method(stacksafe) read &vint:aPointer
        => system'core_routines'__dptr.

    #method(stacksafe) read &bytearray:arr &int:len
        => system'core_routines'__dptr.

    // unsafe code
    #method(stacksafe) int
    [
        #var(int)aValue.
        $self read &vint:aValue.
        
        ^ IntNumber new &int:aValue.
    ]

    // unsafe code
    #method(stacksafe) literal
    [
        #var(int)aLen := system'external'KERNEL32 lstrlenA &int:thePointer.
        #var(literal)aLiteral := LiteralValue new &int:aLen.
        system'external'KERNEL32 lstrcpyA &literal:aLiteral &int:thePointer.
        
        ^ aLiteral.
    ]        

    // unsafe code
    #method(stacksafe) wide
    [
        #var(int)aLen := system'external'KERNEL32 lstrlenW &int:thePointer.
        #var(wide)aLiteral := WideLiteralValue new &int:aLen.
        system'external'KERNEL32 lstrcpyW &wide:aLiteral &int:thePointer.
        
        ^ aLiteral.
    ]
    
    #method(stacksafe) saveTo &bytearray:target &int:offs
    [
        target write &int:offs &int:thePointer.
    ]
    
    #method(stacksafe,bool)is &empty
        = 0 == thePointer.
}

// --- COORD ---

#subject win_coord = system'COORD.

#class(sealed,struct:embeddable) COORD
{
    #field(short)theX.
    #field(short)theY.
    
    #constructor new
    [
        theX := 0.
        theY := 0.
    ]
    
    #method set : anObject
        <= write &win_coord:(anObject win_coord).
    
    #method(stacksafe) write &short:X &short:Y
    [
        theX := X.
        theY := Y.
    ]
    
    #method(stacksafe) write &win_coord:aValue
    [
        #var(short)tmp := aValue X.
        theX := tmp.
        
        tmp := aValue Y.
        theY := tmp.
    ]
    
    #method(stacksafe,outwprop:X) readX &vshort:aRetVal
    [
        aRetVal := theX.
    ]
    
    #method(stacksafe,outwprop:Y) readY &vshort:aRetVal
    [
        aRetVal := theY.
    ]
}

// --- SMALL_RECT --

#subject win_small_rect = system'SMALL_RECT.
                
#class(sealed,struct:embeddable) SMALL_RECT
{
    #field(short)theLeft.
    #field(short)theTop.
    #field(short)theRight.
    #field(short)theBottom.
    
    #method(stacksafe,outwprop:Left) readLeft &vshort:aRetVal
    [
        aRetVal := theLeft.
    ]
    
    #method(stacksafe,outwprop:Top) readTop &vshort:aRetVal
    [
        aRetVal := theTop.
    ]
    
    #method(stacksafe,outwprop:Right) readRight &vshort:aRetVal
    [
        aRetVal := theRight.
    ]
    
    #method(stacksafe,outwprop:Bottom) readBottom &vshort:aRetVal
    [
        aRetVal := theBottom.
    ]
    
    #method set : anObject
        <= write &win_small_rect:(anObject win_small_rect).
    
    #method write &win_small_rect:aValue
    [
        #var(short)tmp := aValue Left.
        theLeft := tmp.
        
        tmp := aValue  Top.
        theTop := tmp.
        
        tmp := aValue Right.
        theRight := tmp.
        
        tmp := aValue Bottom.
        theBottom := tmp.
    ]
}    
