#define system.

#symbol classControl =
{
    readName : anObject &shortarray:aBuffer &out'length:aLength
        => system'internal'load_classname.

    readLength : anObject &out'length:aLength        
        => system'internal'arrptr_getLen.        
                        
//    test &bytearray:anArray &index:anIndex &out'shortarray:aBuffer &out'length:aLength
//        <test>.
//        
//    test
//    [
//        #var(type:int)aLength := 255.
//        #var(type:shortarray,size:256)aBuffer.
//        #var(type:bytearray,size:4)anArray.
//        
//        classControl test &bytearray:anArray &index:0 &out'shortarray:aBuffer &out'length:aLength.
//    ]                
}.

// --- classOp ---

#class(extension) classOp
{
    #method fieldCount
    [
        #var(type:length)aLength := classControl readLength:self.
    ]     
                   
    #method setField &index:anIndex &object:anObject
        => system'internal'arrptr_set.
        
    #method getField &index:anIndex
        => system'internal'arrptr_get.
                                
    #method class
        => system'internal'class_getClass.

    #method name
    [
        #var(type:length)aLength := 255.
        #var(type:shortarray,size:256)aBuffer.
        
        aLength := classControl readName:self &shortarray:aBuffer.
        
        ^ LiteralValue new &length:aLength &shortarray:aBuffer.
    ]

    #method respondsTo &message:aMessage
        => system'internal'class_indexOfMsg.
}

