// --- Handler ---

sealed embeddable struct Handle :: dispatchable(handle)
{
    __ptr theHandle.
    
    stacksafe new handle:h
    [
        theHandle := h
    ]

    stacksafe new int:aHandle
    [
        theHandle := aHandle
    ]

    stacksafe bool equal handle:anOperand 
        = theHandle == anOperand.
}

// --- RTPointer ---

static rt_sync = Object new.

sealed const embeddable struct RTPointer
{
    __dword theIndex.
    
    stacksafe implicit int:n
    [
        theIndex := n.
    ]
    
    constructor new : anObject
    [
        system'core_routines'__rtptr new object:$self object:anObject.
    ]

    int index :: outnprop(readIndexTo).

    stacksafe readIndexTo vint:aRetVal
    [
        aRetVal int := theIndex.
    ]
        
    stacksafe get
        => system'core_routines'__rtptr.

    stacksafe  free
        => system'core_routines'__rtptr.
}

// --- UnsafePointer ---

sealed embeddable struct UnsafePointer :: dispatchable(dirty_ptr)
{
    __ptr thePointer.

//    #constructor new : anObject
//    [
//        $self set:anObject.
//    ]
    
    stacksafe set : anObject
        => system'core_routines'__dptr.
                
    stacksafe set int:aValue
    [
        thePointer := aValue.
    ]
                
    stacksafe offset int:offs
    [
        thePointer := thePointer + offs.
    ]

    stacksafe set dirty_ptr:anObject
        => system'core_routines'__dptr.
                
    stacksafe write int:anObject
        => system'core_routines'__dptr.
                
    stacksafe read vint:aPointer
        => system'core_routines'__dptr.

    stacksafe read bytearray:arr int:len
        => system'core_routines'__dptr.

    // unsafe code
    stacksafe int
    [
        int aValue := 0.
        $self read vint:aValue.
        
        ^ IntNumber new int:aValue
    ]

    // unsafe code
    stacksafe literal
    [
        int     aLen := system'external'KERNEL32 lstrlenA int:thePointer.
        literal aLiteral := LiteralValue new int:aLen.
        system'external'KERNEL32 lstrcpyA literal:aLiteral int:thePointer.
        
        ^ aLiteral
    ]        

    // unsafe code
    stacksafe wide
    [
        int  aLen := system'external'KERNEL32 lstrlenW int:thePointer.
        wide aLiteral := WideLiteralValue new int:aLen.
        system'external'KERNEL32 lstrcpyW wide:aLiteral int:thePointer.
        
        ^ aLiteral
    ]
    
    stacksafe saveTo bytearray:target int:offs
    [
        target write int:offs int:thePointer.
    ]
    
    stacksafe bool is empty
        = 0 == thePointer.
}

// --- COORD ---

type win_coord :: system'COORD.

sealed embeddable struct COORD
{
    short theX.
    short theY.
    
    constructor new
    [
        theX := 0.
        theY := 0.
    ]
    
    set : anObject
        <= write win_coord:anObject.
    
    stacksafe write short:X short:Y
    [
        theX := X.
        theY := Y.
    ]
    
    stacksafe write win_coord:aValue
    [
        short tmp := aValue X.
        theX := tmp.
        
        tmp := aValue Y.
        theY := tmp.
    ]
    
    stacksafe readXTo vshort:aRetVal
    [
        aRetVal short := theX.
    ]
    
    short X :: outwprop(readXTo).
    
    stacksafe readYTo vshort:aRetVal
    [
        aRetVal short := theY.
    ]
    
    short Y :: outwprop(readYTo).
}

// --- SMALL_RECT --

type win_small_rect :: system'SMALL_RECT.
                
sealed embeddable struct SMALL_RECT
{
    short theLeft.
    short theTop.
    short theRight.
    short theBottom.
    
    short Left :: outwprop(readLeftTo).
    
    short Top :: outwprop(readTopTo).
    
    short Right :: outwprop(readRightTo).
    
    short Bottom :: outwprop(readBottomTo).
    
    stacksafe readLeftTo vshort:aRetVal
    [
        aRetVal short := theLeft.
    ]
    
    stacksafe readTopTo vshort:aRetVal
    [
        aRetVal short := theTop.
    ]
    
    stacksafe readRightTo vshort:aRetVal
    [
        aRetVal short := theRight.
    ]
    
    stacksafe readBottomTo vshort:aRetVal
    [
        aRetVal short := theBottom.
    ]
    
    set : anObject
        <= write win_small_rect:anObject.
    
    write win_small_rect:aValue
    [
        short tmp := aValue Left.
        theLeft := tmp.
        
        tmp := aValue  Top.
        theTop := tmp.
        
        tmp := aValue Right.
        theRight := tmp.
        
        tmp := aValue Bottom.
        theBottom := tmp.
    ]
}    
