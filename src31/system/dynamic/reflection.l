//#symbol $classControl =
//{
//    #method(stacksafe) readLength : anObject &vint:aLength        
//        => system'core_routines'arrptr_getLen.        
//}.

// --- classOp ---

extension classROp
{
    int fieldCount
    [
        int aLength := 0.
        system'core_routines'__object readLength object:self vint:aLength.
        
        ^ aLength
    ]     

    stacksafe readFieldCountTo vint:retVal
    [
        system'core_routines'__object readLength object:self vint:retVal
    ]

    stacksafe setField int:anIndex object:anObject        
        => system'core_routines'__object.
        
    stacksafe getField int:anIndex
        => system'core_routines'__object.
        
    getFieldAt : anIndex
        = self~classROp getField int:anIndex.

    stacksafe respondTo mssg:aMessage
        => system'core_routines'__object.

//    stacksafe readMatchedEntries &mssg:aMask &int:aVMTOffset &mssgarray:aResultArray &vint:aResultLength
//    [
//        var class := self::objectOp class.
//
//        int retVal := system'external core_filtervmt
//           &mssg:aMask
//           &ref:class
//           &int:aVMTOffset
//           &int:aResultLength
//           &mssgarray:aResultArray.
//
//        aResultLength := retVal.
//    ]
    
    setFields args:objects
    [
        int i := 0.
        int l := objects length.
        
        while (i < l)
        [
            self setField int:i object:(objects@i).
            
            i := i + 1
        ].
        
        ^ self
    ]
}

// --- manifestOp ---

extension manifestOp
{
    packageNamespace
        => system'core_routines'__object.
        
    packageName
        => system'core_routines'__object.
        
    packageVersion
        => system'core_routines'__object.
        
    packageAuthor
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
