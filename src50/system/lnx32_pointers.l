// --- CallStack ---

/// A call stack
public sealed class CallStack
{
    int[] theStack;
    
    [info:"Creates a current call stack"]
    constructor()
    {
        int stack[64];
        int length := 0;
        
        length := extern elenart.core_callstack_load(stack, 64);
          
        theStack := new int[](length);
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
        int    address := 0;

        byte   buffer[512];
        string callStackText := emptyString;
        for (IntNumber i := 0, i < length, i += 1)
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
//#class(sealed,embeddable,const) RTPointer
//{
//    #field(4,-12)theIndex.
//    
//    #constructor(stacksafe) &int:n
//    [
//        theIndex := n.
//    ]
//    
//    #constructor new : anObject
//    [
//        system'core_routines'__rtptr new &object:$self &object:anObject.
//    ]
//
//    #method(stacksafe,outnprop:index) readIndex &vint:aRetVal
//    [
//        aRetVal := theIndex.
//    ]
//        
//    #method(stacksafe) get
//        => system'core_routines'__rtptr.
//
//    #method(stacksafe)  free
//        => system'core_routines'__rtptr.
//}

// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
    embeddable __ptr thePointer[4];

    embeddable constructor load(int ptr)
    {
        thePointer := ptr
    }

    embeddable constructor()
    {
        thePointer := 0
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
        int  len := extern libc.strlen(self);
        string s := String.allocate(len);
        extern libc.strcpy(s, self);
        
        ^ s
    }        

    // unsafe code
    wide cast()
    {
        int  len := extern libc.wcslen(self);
        wide s := WideString.allocate(len);
        extern libc.wcscpy(s, self);
        
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
        = thePointer == 0;
}

                
