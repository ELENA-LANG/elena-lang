// --- CallStack ---

/// A call stack
public sealed class CallStack
{
    long[] theStack;
    
    [info:"Creates a current call stack"]
    constructor()
    {
        long stack[64];
        int  length := 0;
        
        length := extern elenart.core_callstack_load(stack, 64);
        
        theStack := new long[](length);
        for(int i := 0, i < length, i += 1)
        {
            theStack[i] := stack[i]
        }
    }
    
    /// Generates the call stack literal representation
    string toPrintable()
    {
        int    length := theStack.Length;
        int    bufferLength := 0;
        long   address := 0;

        byte   buffer[512];
        string callStackText := emptyString;
        for (int i := 0, i < length, i += 1)
        {
            address := theStack[i];
            
            bufferLength := extern elenart.LoadAddressInfo(address, buffer, 512);
            if (bufferLength > 0)
            {
                //callStackText += String.fromByteArray(0, bufferLength, buffer).add(forward newLine)
                callStackText := callStackText + String.fromByteArray(0, bufferLength, buffer).add(forward newLine)
            }
        };
        
        ^ callStackText
    }
}

// --- Handler ---

public sealed const struct Handle
{
    embeddable __raw theHandle[4];

    constructor(int n)
    {
        theHandle := n
    }
}

//// --- RTPointer ---
//
//public static rt_sync = new Object();
//
//public sealed const struct RTPointer
//{
//    int theIndex;
//
//    constructor(int n)
//    {
//        theIndex := n
//    }
//    
//    constructor assign(object)
//    {
//        intern system'core_routines'__rtptr.new(self, object)
//    }
//
//    int Index
//    {
//        get()
//        {
//            ^ theIndex
//        }
//    }
//        
//    get()
//        => intern system'core_routines'__rtptr;
//
//    free()
//        => intern system'core_routines'__rtptr;
//}

// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
    embeddable __ptr thePointer[8];

    embeddable constructor load(long ptr)
    {
        thePointer := ptr
    }

    embeddable constructor()
    {
        thePointer := 0l
    }

    embeddable constructor(object ref)
    {
        self.setReference:ref
    }
    
    private setReference(o)
        => intern system'core_routines'__dptr;

    dirty_ptr add(int offs)
    {
        intern system'core_routines'__dptr.readVal(self, ref int val);
        
        val += offs;
        
        dirty_ptr ptr := UnsafePointer.load(val);
        
        ^ ptr
    }

    get int Value()
        = cast int(self);

    // unsafe code
    int cast()
    {
        intern system'core_routines'__dptr.readVal(self, ref int val);
        
        ^ val
    }

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

    copyTo(byte[] target, int len)
    {
        intern system'core_routines'__dptr.copyTo(self, target, len);
    }
            
    bool isEmpty()
        = thePointer == 0l;
}

//// --- COORD ---
//
//public sealed struct COORD
//{
//    short theX;
//    short theY;
//    
//    embeddable constructor()
//    {
//        theX := 0;
//        theY := 0;
//    }
//    
//    write(short X, short Y)
//    {
//        theX := X;
//        theY := Y
//    }
//    
//    write(COORD r)
//    {
//        short tmp := r.X;
//        theX := tmp;
//        
//        tmp := r.Y;
//        theY := tmp
//    }
//    
//    short X
//    {
//        get()
//        {
//            ^ theX
//        }
//    }
//    
//    short Y
//    {    
//        get()
//        {
//            ^ theY
//        }
//    }
//}
//
//// --- SMALL_RECT --
//
//public sealed struct SMALL_RECT
//{
//    short theLeft;
//    short theTop;
//    short theRight;
//    short theBottom;
//    
//    short Left
//    {
//        get()
//        {
//            ^ theLeft
//        }
//    }
//    
//    short Top
//    {
//        get()
//        {
//            ^ theTop
//        }
//    }
//    
//    short Right
//    {
//        get()
//        {
//            ^ theRight
//        }        
//    }
//    
//    short Bottom
//    {
//        get()
//        {
//            ^ theBottom
//        }
//    }
//    
//    write(SMALL_RECT r)
//    {
//        short tmp := r.Left;
//        theLeft := tmp;
//        
//        tmp := r.Top;
//        theTop := tmp;
//        
//        tmp := r.Right;
//        theRight := tmp;
//        
//        tmp := r.Bottom;
//        theBottom := tmp;
//    }
//}    
