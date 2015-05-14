#define system.

#subject(class:system'Message)mssg_mask.

#symbol $classControl =
{
    readName : anObject &bytearray:aBuffer &vlength:aLength
        => system'internal'load_classname.

    readLength : anObject &vint:aLength        
        => system'internal'arrptr_getLen.        
}.

// --- classOp ---

#class(extension) classOp
{
    #method fieldCount
    [
        #var(type:int)aLength.
        $classControl readLength:self &vint:aLength.
        
        ^ IntNumber new &int:aLength.
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
        #var(type:bytearray,size:256)aBuffer.
        
        $classControl readName:self &bytearray:aBuffer &vlength:aLength.
        
        ^ LiteralValue $new &length:aLength &index:0 &bytearray:aBuffer.
    ]

    #method respondsTo &message:aMessage
        => system'internal'class_indexOfMsg.
        
    #method read &mssg_mask:aMask &index:aVMTOffset &mssgarray:aResultArray &vint:aResultLength
        => system'internal'class_filterMsg.
}
