#define system.

#symbol classControl =
{
    readLength &object:anObject &out'int:aLength
        <class_getLen>.
        
    read &index:anIndex &object:anObject
        <class_get>.
        
    readName : anObject &out'shortarray:aBuffer &out'length:aLength
        <class_getName>.
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

    #method fieldCount
    [
        #var(type:int)aCount.
        classControl readLength &object:self &out'int:aCount.
        
        ^ IntNumber new &int:aCount.
    ]

    #method getFieldAt &index:anIndex
        = classControl read &index:anIndex &object:self.
}