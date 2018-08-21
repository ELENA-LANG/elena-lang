// --- Handler ---

public sealed const struct Handle
{
    int theHandle.

    constructor new(handle h)
    [
        theHandle := h
    ]
    
    constructor new(int h)
    [
        theHandle := h
    ]

    constructor(int n)
    [
        theHandle := n.
    ]

    cast T<int> = theHandle.

    bool equal(handle anOperand)
        = theHandle == anOperand.
}

// --- RTPointer ---

public static rt_sync = Object new.

public sealed const struct RTPointer
{
    int theIndex.

    stacksafe constructor(int n)
    [
        theIndex := n.
    ]
    
    constructor new : anObject
    [
        system'core_routines'__rtptr new(self, anObject).
    ]

    int rvprop index :: readIndexTo.

    stacksafe readIndexTo(ref<int> retVal)
    [
        retVal value := theIndex.
    ]
        
    stacksafe get
        => system'core_routines'__rtptr.

    stacksafe  free
        => system'core_routines'__rtptr.
}

// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
    __ptr thePointer.

    /*embeddable */constructor load(int ptr)
    [
        thePointer := ptr.
    ]

    constructor(object ref)
    [
        self setReference:ref.
    ]
    
    private setReference:anObject
        => system'core_routines'__dptr.
                                         
    dirty_ptr stacksafe add(int offs)
    [
        dirty_ptr ptr := self.
        
        ptr := ptr + 1.
        
        ^ ptr
    ]

//    stacksafe write int:anObject
//        => system'core_routines'__dptr.
                
    stacksafe read(ref<int> retVal)
        => system'core_routines'__dptr.

    stacksafe read(ByteArray arr, int len)
        => system'core_routines'__dptr.

    // unsafe code
    stacksafe int
    [
        int val := 0.
        self read(&val).
        
        ^ val
    ]

    // unsafe code
    T<literal> literal
    [
        int     aLen := system'external'KERNEL32 lstrlenA(self).
        literal aLiteral := LiteralValue new(aLen).
        system'external'KERNEL32 lstrcpyA(aLiteral, self).
        
        ^ aLiteral
    ]        

    // unsafe code
    T<wide> wide
    [
        int  aLen := system'external'KERNEL32 lstrlenW(self).
        wide aLiteral := WideLiteralValue new(aLen).
        system'external'KERNEL32 lstrcpyW(aLiteral, self).
        
        ^ aLiteral
    ]
    
//    stacksafe saveTo bytearray:target int:offs
//    [
//        target write int:offs int:thePointer.
//    ]
//    
//    stacksafe saveTo intarray:target int:offs
//    [
//        target write int:offs int:thePointer.
//    ]
    
    stacksafe bool isEmpty
        = 0 == thePointer.
}

// --- COORD ---

public sealed struct COORD
{
    short theX.
    short theY.
    
    constructor new
    [
        theX := 0.
        theY := 0.
    ]
    
    stacksafe write(short X, short Y)
    [
        theX := X.
        theY := Y.
    ]
    
    stacksafe write(COORD r)
    [
        short tmp := r X.
        theX := tmp.
        
        tmp := r Y.
        theY := tmp.
    ]
    
    stacksafe readXTo(ref<short> aRetVal)
    [
        aRetVal value := theX.
    ]
    
    short rvprop X :: readXTo.
    
    stacksafe readYTo(ref<short> aRetVal)
    [
        aRetVal value := theY.
    ]
    
    short rvprop Y :: readYTo.
}

// --- SMALL_RECT --

public sealed struct SMALL_RECT
{
    short theLeft.
    short theTop.
    short theRight.
    short theBottom.
    
    short rvprop Left   :: readLeftTo.
    
    short rvprop Top    :: readTopTo.
    
    short rvprop Right  :: readRightTo.
    
    short rvprop Bottom :: readBottomTo.
    
    stacksafe readLeftTo(ref<short> aRetVal)
    [
        aRetVal value := theLeft.
    ]
    
    stacksafe readTopTo(ref<short> aRetVal)
    [
        aRetVal value := theTop.
    ]
    
    stacksafe readRightTo(ref<short> aRetVal)
    [
        aRetVal value := theRight.
    ]
    
    stacksafe readBottomTo(ref<short> aRetVal)
    [
        aRetVal value := theBottom.
    ]
    
    write(SMALL_RECT r)
    [
        short tmp := r Left.
        theLeft := tmp.
        
        tmp := r  Top.
        theTop := tmp.
        
        tmp := r Right.
        theRight := tmp.
        
        tmp := r Bottom.
        theBottom := tmp.
    ]
}    
