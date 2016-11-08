#import system.

//#symbol $classControl =
//{
//    #method(stacksafe) readLength : anObject &vint:aLength        
//        => system'core_routines'arrptr_getLen.        
//}.

// --- classOp ---

#class(extension) classROp
{
    #method class
        => system'core_routines'__object.

    #method(literal) className
    [
        #var class := self::classROp class.
        
        #var(byte:256)aBuffer.
        #var(int)aLength := system'external LoadClassName 
            &ref:class
            &bytearray:aBuffer
            &int:255.
        
        (aLength == 0)
            ? [ ^ "Unknown class". ].
        
        ^ LiteralValue $new &int:0 &int:aLength &bytearray:aBuffer.
    ]    
    
    #method(int) fieldCount
    [
        #var(int)aLength.                
        system'core_routines'__object readLength &object:self &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]     

    #method(stacksafe) readFieldCount &vint:retVal
    [
        system'core_routines'__object readLength &object:self &vint:retVal.
    ]

    #method(stacksafe) setField &int:anIndex &object:anObject        
        => system'core_routines'__object.
        
    #method(stacksafe) getField &int:anIndex
        => system'core_routines'__object.
        
    #method getFieldAt : anIndex
        = self::classROp getField &int:(anIndex int).

    #method(stacksafe) respondTo &mssg:aMessage
        => system'core_routines'__object.

    #method(stacksafe) readMatchedEntries &mssg:aMask &int:aVMTOffset &mssgarray:aResultArray &vint:aResultLength
    [
        #var class := self::classROp class.

        #var(int)retVal := system'external core_filtervmt
           &mssg:aMask
           &ref:class
           &int:aVMTOffset
           &int:aResultLength
           &mssgarray:aResultArray.

        aResultLength := retVal.
    ]
    
    #method setFields &args:objects
    [
        #var(int)i := 0.
        #var(int)l := objects length.
        
        #loop (i < l)?
        [
            self setField &int:i &object:(objects@i).
            
            i += 1.
        ].
        
        ^ self.
    ]
}

// --- manifestOp ---

#class(extension) manifestOp
{
    #method packageNamespace
        => system'core_routines'__object.
        
    #method packageName
        => system'core_routines'__object.
        
    #method packageVersion
        => system'core_routines'__object.
        
    #method packageAuthor
        => system'core_routines'__object.
}

//// --- subjectOp ---
//
//#class(extension:subject) subjectROp
//{
//    #method(stacksafe) $equalSubject : anObject
//        => system'core_routines'subject_equal.
//    
//    /// obsolete
//    #method name
//        = self literal.
//}
