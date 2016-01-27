// ==== ver 3.4.1 ===

#import system.

#symbol $classControl =
{
    #method(stacksafe) readLength : anObject &vint:aLength        
        => system'core_routines'arrptr_getLen.        
}.

// --- classOp ---

#class(extension) classROp
{
    #method(type:int) fieldCount
    [
        #var(type:int)aLength.
        $classControl readLength:self &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]     
                   
    #method(stacksafe) setField &int:anIndex &object:anObject
        => system'core_routines'arrptr_set.
        
    #method(stacksafe) getField &int:anIndex
        => system'core_routines'arrptr_get.
        
    #method getFieldAt : anIndex
        = self::classROp getField &int:(anIndex int).
                                
    #method class
        => system'core_routines'class_getClass.

    /// obsolete
    #method name
        = self::classROp className.

    #method className
    [
        #var class := self::classROp class.
        
        #var(type:bytearray,size:256)aBuffer.
        #var(type:int)aLength := system'external LoadClassName 
            &ref:class
            &bytearray:aBuffer
            &int:255.
        
        (aLength == 0)
            ? [ #throw NotSupportedException new:"Reflection functionality is not supported". ].
        
        ^ LiteralValue $new &int:0 &int:aLength &bytearray:aBuffer.
    ]

    #method(stacksafe) respondsTo &message:aMessage
        => system'core_routines'class_indexOfMsg.
        
    #method(stacksafe) read &mssg_mask:aMask &index:aVMTOffset &mssgarray:aResultArray &vint:aResultLength
        => system'core_routines'class_filterMsg.
}

// --- subjectOp ---

#class(extension:subject) subjectROp
{
    #method(stacksafe) $equalSubject : anObject
        => system'core_routines'subject_equal.
    
    /// obsolete
    #method(stacksafe) name
        = self literal.
}
