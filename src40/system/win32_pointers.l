// --- Handler ---

public sealed const struct Handle
{
    embeddable __raw theHandle[4];

//    constructor new(handle h)
//    [
//        theHandle := h
//    ]
//    
//    constructor new(int h)
//    [
//        theHandle := h
//    ]
//
//    constructor(int n)
//    [
//        theHandle := n.
//    ]
//
//    cast T<int> = theHandle.
//
//    bool equal(handle anOperand)
//        = theHandle == anOperand.
//        
//    bool equal(int anOperand)
//        = theHandle == anOperand.
}

//// --- RTPointer ---
//
//public static rt_sync = Object new.
//
//public sealed const struct RTPointer
//{
//    int theIndex.
//
//    constructor(int n)
//    [
//        theIndex := n.
//    ]
//    
//    constructor new : anObject
//    [
//        system'core_routines'__rtptr new(self, anObject).
//    ]
//
//    int rvprop index :: readIndexTo.
//
//    readIndexTo(ref<int> retVal)
//    [
//        retVal value := theIndex.
//    ]
//        
//    get
//        => system'core_routines'__rtptr.
//
//    free
//        => system'core_routines'__rtptr.
//}
//
//// --- UnsafePointer ---
//
//public sealed const struct UnsafePointer
//{
//    __ptr thePointer.
//
//    embeddable constructor load(int ptr)
//    [
//        thePointer := ptr.
//    ]
//
//    constructor
//    [
//        thePointer := 0.
//    ]
//
//    constructor(object ref)
//    [
//        self setReference:ref.
//    ]
//    
//    private setReference:anObject
//        => system'core_routines'__dptr.
//                                         
//    dirty_ptr add(int offs)
//    [
//        dirty_ptr ptr := self.
//        
//        ptr := ptr + 1.
//        
//        ^ ptr
//    ]
//
////    stacksafe write int:anObject
////        => system'core_routines'__dptr.
//                
//    read(ref<int> retVal)
//        => system'core_routines'__dptr.
//
//    read(ByteArray arr, int len)
//        => system'core_routines'__dptr.
//
//    // unsafe code
//    int
//    [
//        int val := 0.
//        self read(&val).
//        
//        ^ val
//    ]
//
//    // unsafe code
//    T<literal> literal
//    [
//        int     aLen := system'external'KERNEL32 lstrlenA(self).
//        literal aLiteral := LiteralValue new(aLen).
//        system'external'KERNEL32 lstrcpyA(aLiteral, self).
//        
//        ^ aLiteral
//    ]        
//
//    // unsafe code
//    T<wide> wide
//    [
//        int  aLen := system'external'KERNEL32 lstrlenW(self).
//        wide aLiteral := WideLiteralValue new(aLen).
//        system'external'KERNEL32 lstrcpyW(aLiteral, self).
//        
//        ^ aLiteral
//    ]
//    
////    stacksafe saveTo bytearray:target int:offs
////    [
////        target write int:offs int:thePointer.
////    ]
////    
////    stacksafe saveTo intarray:target int:offs
////    [
////        target write int:offs int:thePointer.
////    ]
//    
//    bool isEmpty
//        = 0 == thePointer.
//}
//
//// --- COORD ---
//
//public sealed struct COORD
//{
//    short theX.
//    short theY.
//    
//    constructor new
//    [
//        theX := 0.
//        theY := 0.
//    ]
//    
//    write(short X, short Y)
//    [
//        theX := X.
//        theY := Y.
//    ]
//    
//    write(COORD r)
//    [
//        short tmp := r X.
//        theX := tmp.
//        
//        tmp := r Y.
//        theY := tmp.
//    ]
//    
//    readXTo(ref<short> aRetVal)
//    [
//        aRetVal value := theX.
//    ]
//    
//    short rvprop X :: readXTo.
//    
//    readYTo(ref<short> aRetVal)
//    [
//        aRetVal value := theY.
//    ]
//    
//    short rvprop Y :: readYTo.
//}
//
//// --- SMALL_RECT --
//
//public sealed struct SMALL_RECT
//{
//    short theLeft.
//    short theTop.
//    short theRight.
//    short theBottom.
//    
//    short rvprop Left   :: readLeftTo.
//    
//    short rvprop Top    :: readTopTo.
//    
//    short rvprop Right  :: readRightTo.
//    
//    short rvprop Bottom :: readBottomTo.
//    
//    readLeftTo(ref<short> aRetVal)
//    [
//        aRetVal value := theLeft.
//    ]
//    
//    readTopTo(ref<short> aRetVal)
//    [
//        aRetVal value := theTop.
//    ]
//    
//    readRightTo(ref<short> aRetVal)
//    [
//        aRetVal value := theRight.
//    ]
//    
//    readBottomTo(ref<short> aRetVal)
//    [
//        aRetVal value := theBottom.
//    ]
//    
//    write(SMALL_RECT r)
//    [
//        short tmp := r Left.
//        theLeft := tmp.
//        
//        tmp := r  Top.
//        theTop := tmp.
//        
//        tmp := r Right.
//        theRight := tmp.
//        
//        tmp := r Bottom.
//        theBottom := tmp.
//    ]
//}    
