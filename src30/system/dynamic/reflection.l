//#define system.
//
//#symbol classOp =
//{
//    getCount &type'object:anObject &out'type'int:aLength
//        <class_getLen>.
//        
//    getAt &type'object:anObject &type'index:anIndex
//        <class_get>.
//}.
//
//// --- EClass ---
//
//#class(role) EClass
//{
//    #method Name
//        <class_getName>.
//
//    #method Count
//    [
//        #var(type:int)aCount := classOp getCount &type'object:self.
//        
//        ^ IntNumber new &type'int:aCount.
//    ]
//
//    #method getAt : anIndex
//        = classOp getAt &type'object:self &type'index::anIndex.
//}