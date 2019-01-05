//////#symbol $classControl =
//////{
//////    #method(stacksafe) readLength : anObject &vint:aLength        
//////        => system'core_routines'arrptr_getLen.        
//////}.
////
////type mssgarray = array<Message>.

// --- classOp ---

public extension classROp
{
//    int fieldCount
//    [
//        int aLength := 0.
//        system'core_routines'__object readLength(self, &aLength).
//        
//        ^ aLength
//    ]     
//
//    readFieldCountTo(ref<int> retVal)
//    [
//        system'core_routines'__object readLength(self, retVal)
//    ]
//
//    setField(int anIndex, object anObject)
//        => system'core_routines'__object.
//        
//    getField(int anIndex)
//        => system'core_routines'__object.
//        
////    stacksafe respondTo mssg:aMessage
////        => system'core_routines'__object.
//
//    Array<Message> __messages
//    [
//        dirty_ptr class := self~objectOp __class.
//
//        T<int,256> tmp_array.
//        int retVal := system'external core_readvmt(0,class,256,T<IntArray>(tmp_array)).
//
//        Array<Message> array := V<Message>(retVal).
//        int i := 0.
//        while (i < retVal)
//        [
//           T<Message> message.
//           system'core_routines'__message copy(message, tmp_array[i]).
//            
//           array[i] := message.
//            
//           i += 1.
//        ].
//
//        ^ array.
//    ]
    
    setFields(object[] objects)
    {
        int l := objects.Length;
        for (int i := 0, i < l, i += 1) {
            self.setField(i, objects[i])
        };
        
        ^ self
    }
}

////// --- manifestOp ---
////
////extension manifestOp
////{
////    packageNamespace
////        => system'core_routines'__object.
////        
////    packageName
////        => system'core_routines'__object.
////        
////    packageVersion
////        => system'core_routines'__object.
////        
////    packageAuthor
////        => system'core_routines'__object.
////}
////
//////// --- subjectOp ---
//////
//////#class(extension:subject) subjectROp
//////{
//////    #method(stacksafe) $equalSubject : anObject
//////        => system'core_routines'subject_equal.
//////    
//////    /// obsolete
//////    #method name
//////        = self literal.
//////}
