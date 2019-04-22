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

    constructor(int n)
    {
        theHandle := n
    }

//    cast T<int> = theHandle.
//
//    bool equal(handle anOperand)
//        = theHandle == anOperand.
//        
//    bool equal(int anOperand)
//        = theHandle == anOperand.
}

// --- RTPointer ---

public static rt_sync = new Object();

public sealed const struct RTPointer
{
    int theIndex;

    constructor(int n)
    {
        theIndex := n
    }
    
    constructor assign(object)
    {
        intern system'core_routines'__rtptr.new(self, object)
    }

    Index
    {
        int get()
        {
            ^ theIndex
        }
    }
        
    get()
        => system'core_routines'__rtptr;

    free()
        => system'core_routines'__rtptr;
}

// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
    embeddable __ptr thePointer[4];

//    embeddable constructor load(int ptr)
//    [
//        thePointer := ptr.
//    ]

    embeddable constructor()
    {
        thePointer := 0
    }

    embeddable constructor(object ref)
    {
        self.setReference:ref
    }
    
    private setReference(o)
        => system'core_routines'__dptr;
                                         
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

    // unsafe code
    string cast()
    {
        int  len := extern KERNEL32.lstrlenA(self);
        string s := String.allocate(len);
        extern KERNEL32.lstrcpyA(s, self);
        
        ^ s
    }        

    // unsafe code
    wide cast()
    {
        int  len := extern KERNEL32.lstrlenW(self);
        wide s := WideString.allocate(len);
        extern KERNEL32.lstrcpyW(s, self);
        
        ^ s
    }
    
    saveTo(byte[] target, int offs)
    {
        intern system'core_routines'__dptr.readVal(self, ref int val);
        
        target.write(offs, val)
    }
    
    savePtrTo(byte[] target, int offs)
    {
        intern system'core_routines'__dptr.read(self, ref int val);
        
        target.write(offs, val)
    }
    
//    stacksafe saveTo intarray:target int:offs
//    [
//        target write int:offs int:thePointer.
//    ]
    
    bool isEmpty()
        = thePointer == 0;
}

// --- COORD ---

public sealed struct COORD
{
    short theX;
    short theY;
    
    embeddable constructor()
    {
        theX := 0;
        theY := 0;
    }
    
    write(short X, short Y)
    {
        theX := X;
        theY := Y
    }
    
    write(COORD r)
    {
        short tmp := r.X;
        theX := tmp;
        
        tmp := r.Y;
        theY := tmp
    }
    
    X
    {
        short get()
        {
            ^ theX
        }
    }
    
    Y
    {    
        short get()
        {
            ^ theY
        }
    }
}

// --- SMALL_RECT --

public sealed struct SMALL_RECT
{
    short theLeft;
    short theTop;
    short theRight;
    short theBottom;
    
    Left
    {
        short get()
        {
            ^ theLeft
        }
    }
    
    Top
    {
        short get()
        {
            ^ theTop
        }
    }
    
    Right
    {
        short get()
        {
            ^ theRight
        }        
    }
    
    Bottom
    {
        get()
        {
            ^ theBottom
        }
    }
    
    write(SMALL_RECT r)
    {
        short tmp := r.Left;
        theLeft := tmp;
        
        tmp := r.Top;
        theTop := tmp;
        
        tmp := r.Right;
        theRight := tmp;
        
        tmp := r.Bottom;
        theBottom := tmp;
    }
}    
