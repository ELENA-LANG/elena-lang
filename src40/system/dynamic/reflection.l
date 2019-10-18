import system'collections;

////#symbol $classControl =
////{
////    #method(stacksafe) readLength : anObject &vint:aLength        
////        => system'core_routines'arrptr_getLen.        
////}.

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

    setField(int index, object object)
        => system'core_routines'__object;
        
//    getField(int anIndex)
//        => system'core_routines'__object.
        
    bool respondTo(mssg m)
        => system'core_routines'__object;

    Message[] __getMessages()
    {
        dirty_ptr class := self/*.__getClass()*/;

        int tmp_array[256];
        int retVal := extern elenart.core_readvmt(0,class,256,tmp_array);

        List<Message> list := new List<Message>();
        for(int i := 1, i < retVal, i += 1)
        {
           ifnot (tmp_array[i].anyMask(60h))
           {
               Message message;
               intern system'core_routines'__message.copy(message, tmp_array[i]);
               
               list.append(message);
           };
        };

        ^ list.Value
    }
    
    MessageName[] __getProperties()
    {
        dirty_ptr class := self/*.__getClass()*/;

        int tmp_array[256];
        int retVal := extern elenart.core_readvmt(0,class,256,tmp_array);

        auto list := new List<MessageName>();
        for(int i := 0, i < retVal, i += 1)
        {
           int id := tmp_array[i];
           if (id.allMask(40h))
           {
               int count := id && 01Fh;
               if (count == 0) {
                   Message message;
                   intern system'core_routines'__message.copy(message, tmp_array[i]);
                   
                   MessageName prop := message.MessageName;
                   
                   list.append(prop);
               }
           };
        };

        ^ list.Value
    }
    
    setFields(params object[] objects)
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
