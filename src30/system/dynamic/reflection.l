//#define system.
//
//#symbol classControl =
//{
//    readName : anObject &chararray:aBuffer &out'length:aLength
//        => system'internal'load_classname.
//
//    readLength : anObject &out'length:aLength        
//        => system'internal'arrptr_getLen.        
//}.
//
//// --- classOp ---
//
//#class(extension) classOp
//{
//    #method fieldCount
//    [
//        #var(type:length)aLength := classControl readLength:self.
//    ]     
//                   
//    #method setField &index:anIndex &object:anObject
//        => system'internal'arrptr_set.
//        
//    #method getField &index:anIndex
//        => system'internal'arrptr_get.
//                                
//    #method class
//        => system'internal'class_getClass.
//
//    #method name
//    [
//        #var(type:length)aLength := 255.
//        #var(type:chararray,size:256)aBuffer.
//        
//        aLength := classControl readName:self &chararray:aBuffer.
//        
//        ^ LiteralValue new &length:aLength &chararray:aBuffer.
//    ]
//
//    #method respondsTo &message:aMessage
//        => system'internal'class_indexOfMsg.
//}
