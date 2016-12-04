// --- Handler ---

#class(embeddable,sealed,dispatchable:handle) Handle
{
    #field(4,-12)theHandle.

//    #constructor(stacksafe) &int:n
//    [
//        theHandle := n.
//    ]
//    
//    #constructor(stacksafe) new &handle:h
//    [
//        theHandle := h.
//    ]
//
//    #constructor(stacksafe) new &int:aHandle
//    [
//        theHandle := aHandle.
//    ]
//
//    #constructor min
//        = 0.
//
//    #method(bool,stacksafe) equal &handle:anOperand 
//        = theHandle == anOperand.
}

// --- RTPointer ---

#static rt_sync = Object new.

#class(sealed,embeddable,const) RTPointer
{
    #field(4,-12)theIndex.
    
    #constructor(stacksafe) &int:n
    [
        theIndex := n.
    ]
    
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

#class(embeddable,sealed,dispatchable:dirty_ptr) UnsafePointer
{
    #field(4,-12)thePointer.

//    #constructor new : anObject
//    [
//        $self set:anObject.
//    ]
    
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

//    // unsafe code
//    #method(stacksafe) literal
//    [
//        #var(int)aLen := system'external'KERNEL32 lstrlenA &int:thePointer.
//        #var(literal)aLiteral := LiteralValue new &int:aLen.
//        system'external'KERNEL32 lstrcpyA &literal:aLiteral &int:thePointer.
//        
//        ^ aLiteral.
//    ]        
//
//    // unsafe code
//    #method(stacksafe) wide
//    [
//        #var(int)aLen := system'external'KERNEL32 lstrlenW &int:thePointer.
//        #var(wide)aLiteral := WideLiteralValue new &int:aLen.
//        system'external'KERNEL32 lstrcpyW &wide:aLiteral &int:thePointer.
//        
//        ^ aLiteral.
//    ]
    
    #method(stacksafe) saveTo &bytearray:target &int:offs
    [
        target write &int:offs &int:thePointer.
    ]
    
    #method(stacksafe,bool)is &empty
        = 0 == thePointer.
}

                
