#define system.

#symbol classControl =
{
    readName : anObject &out'shortarray:aBuffer &out'length:aLength
        <load_classname>.
        
    test &bytearray:anArray &index:anIndex &out'shortarray:aBuffer &out'length:aLength
        <test>.
        
    test
    [
        #var(type:int)aLength := 255.
        #var(type:shortarray,size:256)aBuffer.
        #var(type:bytearray,size:4)anArray.
        
        classControl test &bytearray:anArray &index:0 &out'shortarray:aBuffer &out'length:aLength.
    ]                
}.

// --- classOp ---

#class(extension) classOp
{
    #method Class
        <class_getClass>.

    #method Name
    [
        #var(type:int)aLength := 255.
        #var(type:shortarray,size:256)aBuffer.
        
        classControl readName:self &out'shortarray:aBuffer &out'length:aLength.
        
        ^ LiteralValue new &length:aLength &shortarray:aBuffer.
    ]

    #method respondsTo &message:aMessage
        <class_indexOfMsg>.
}

