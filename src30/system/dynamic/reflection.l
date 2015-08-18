#import system.

#symbol $classControl =
{
    #method(stacksafe) readName : anObject &bytearray:aBuffer &vint:aLength
        => system'internal'load_classname.

    #method(stacksafe) readLength : anObject &vint:aLength        
        => system'internal'arrptr_getLen.        
}.

#symbol $subjectControl =
{
    #method(stacksafe) readName:anObject &bytearray:aBuffer &vint:aLength
        => system'internal'load_subjname.
}.

// --- classOp ---

#class(extension) classROp
{
    #method fieldCount
    [
        #var(type:int)aLength.
        $classControl readLength:self &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]     
                   
    #method(stacksafe) setField &int:anIndex &object:anObject
        => system'internal'arrptr_set.
        
    #method(stacksafe) getField &int:anIndex
        => system'internal'arrptr_get.
                                
    #method class
        => system'internal'class_getClass.

    #method name
    [
        #var(type:int)aLength := 255.
        #var(type:bytearray,size:256)aBuffer.
        
        $classControl readName:self &bytearray:aBuffer &vint:aLength.
        
        (aLength == 0)
            ? [ #throw NotSupportedException new:"Reflection functionality is not supported". ].
        
        ^ LiteralValue $new &int:0 &int:aLength &bytearray:aBuffer.
    ]

    #method respondsTo &message:aMessage
        => system'internal'class_indexOfMsg.
        
    #method read &mssg_mask:aMask &index:aVMTOffset &mssgarray:aResultArray &vint:aResultLength
        => system'internal'class_filterMsg.
}

// --- subjectOp ---

#class(extension:subject) subjectROp
{
    #method(stacksafe) $equalSubject : anObject
        => system'internal'subject_equal.
    
    #method name
    [
        #var(type:int)aLength := 255.
        #var(type:bytearray,size:256)aBuffer.
        
        $subjectControl readName:self &bytearray:aBuffer &vint:aLength.
        
        ^ LiteralValue $new &int:0 &int:aLength &bytearray:aBuffer.
    ]
}