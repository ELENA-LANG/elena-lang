////#symbol $classControl =
////{
////    #method(stacksafe) readLength : anObject &vint:aLength        
////        => system'core_routines'arrptr_getLen.        
////}.
//
//type mssgarray = array<Message>.

// --- classOp ---

public extension classROp
{
    int fieldCount
    [
        int aLength := 0.
        system'core_routines'__object readLength(self, &aLength).
        
        ^ aLength
    ]     

    stacksafe readFieldCountTo(ref<int> retVal)
    [
        system'core_routines'__object readLength(self, retVal)
    ]

    stacksafe setField(int anIndex, object anObject)
        => system'core_routines'__object.
        
    stacksafe getField(int anIndex)
        => system'core_routines'__object.
        
//    stacksafe respondTo mssg:aMessage
//        => system'core_routines'__object.
//
//    array<Message> __messages
//    [
//        var class := self~objectOp class_ref.
//
//        int<256> tmp_array.
//        int retVal := system'external core_readvmt
//           int:0
//           ref:class
//           int:256
//           intarray:tmp_array.
//
//        array<Message> array := __array<Message>(retVal).
//        int i := 0.
//        while (i < retVal)
//        [
//           array[i] := Message $new int:(tmp_array[i]).
//            
//           i += 1.
//        ].
//
//        ^ array.
//    ]
    
    generic setFields(V<object> objects)
    [
        int i := 0.
        int l := objects length.
        
        while (i < l)
        [
            self setField(i, objects[i]).
            
            i := i + 1
        ].
        
        ^ self
    ]
}

//// --- manifestOp ---
//
//extension manifestOp
//{
//    packageNamespace
//        => system'core_routines'__object.
//        
//    packageName
//        => system'core_routines'__object.
//        
//    packageVersion
//        => system'core_routines'__object.
//        
//    packageAuthor
//        => system'core_routines'__object.
//}
//
////// --- subjectOp ---
////
////#class(extension:subject) subjectROp
////{
////    #method(stacksafe) $equalSubject : anObject
////        => system'core_routines'subject_equal.
////    
////    /// obsolete
////    #method name
////        = self literal.
////}
