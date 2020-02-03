// --- Handler ---

public sealed const struct Handle
{
    embeddable __raw theHandle[4];

////    constructor new(handle h)
////    [
////        theHandle := h
////    ]
////    
////    constructor new(int h)
////    [
////        theHandle := h
////    ]

    constructor(int n)
    {
        theHandle := n
    }
    
////    cast T<int> = theHandle.
////
////    bool equal(handle anOperand)
////        = theHandle == anOperand.
////        
////    bool equal(int anOperand)
////        = theHandle == anOperand.
//
//    #constructor min
//        = 0.
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

// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
    embeddable __ptr thePointer[4];

//    #constructor new : anObject
//    [
//        $self set:anObject.
//    ]
    
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
                
//    dirty_ptr add(int offs)
//    {
//        intern system'core_routines'__dptr.readVal(self, ref int val);
//        
//        val += offs;
//        
//        dirty_ptr ptr := UnsafePointer.load(val);
//        
//        ^ ptr
//    }

    get int Value()
        = cast int(self);

    // unsafe code
    int cast()
    {
        intern system'core_routines'__dptr.readVal(self, ref int val);
        
        ^ val
    }

//    // unsafe code
//    string cast()
//    {
//        int  len := extern KERNEL32.lstrlenA(self);
//        string s := String.allocate(len);
//        extern KERNEL32.lstrcpyA(s, self);
//        
//        ^ s
//    }        

//    // unsafe code
//    wide cast()
//    {
//        int  len := extern KERNEL32.lstrlenW(self);
//        wide s := WideString.allocate(len);
//        extern KERNEL32.lstrcpyW(s, self);
//        
//        ^ s
//    }
    
//    saveTo(byte[] target, int offs)
//    {
//        intern system'core_routines'__dptr.readVal(self, ref int val);
//        
//        target.write(offs, val)
//    }
//    
//    savePtrTo(byte[] target, int offs)
//    {
//        intern system'core_routines'__dptr.read(self, ref int val);
//        
//        target.write(offs, val)
//    }
//
//    copyTo(byte[] target, int len)
//    {
//        intern system'core_routines'__dptr.copyTo(self, target, len);
//    }
//            
//    bool isEmpty()
//        = thePointer == 0;
}

                
