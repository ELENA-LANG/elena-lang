// --- Group ---

public mixin Group
{
    object[] items;
    
    internal constructor allocate(int len)
        = new object[](len);
        
    constructor load(params object[] objects)
        <= allocate(objects.Length)
    {
        self.setFields(params objects)
    }
    
    dispatch() => system'core_routines'__group;
}

////// --- Cast ---
////
////mixin Cast
////{
////    object<> $item.
////    
////    stacksafe constructor $new int:aLength
////        = object<>(aLength).
////        
////    constructor new args:objects
////        <= $new int(objects length);
////    [
////        $self setFields args:objects.
////    ]
////    
////    dispatch => system'core_routines'__castover.
////}
//
//// --- extensions ---
//
//public extension dynamicOp
//{
//    mixInto:role
//        = Extension::(role, self).
//        
//    groupWith:object
//    [
//        if(nil == object)
//            [ ^ self ].
//        
//        if(Group == self __class)
//            [ 
//                int length := self fieldCount.
//            
//                var retVal := Group newInteral(length + 1).
//                                
//                int i := 0.
//                
//                while (i < length)
//                [
//                    retVal setField(i, self getField(i)).
//                    
//                    i := i + 1.
//                ].
//                
//                retVal setField(length, object).
//                
//                ^ retVal
//            ];
//            [ 
//                if (Group == object __class)
//                    [
//                        int length := object fieldCount.
//                    
//                        var retVal := Group newInteral(length + 1).
//                                        
//                        int i := 0.
//                        
//                        while (i < length)
//                        [
//                            retVal setField(i + 1, self getField(i)).
//                            
//                            i := i + 1
//                        ].
//                        
//                        retVal setField(0, object).
//                        
//                        ^ retVal
//                    ];
//                    [ ^ Group new(self,object) ]
//            ].
//    ]        
//        
////    castWith:object
////    [
////        if ($nil == object)
////            [ ^ self ].
////        
////        if(self class_ref == Cast)
////            [ 
////                int length := self fieldCount.
////            
////                var retVal := Cast $new int(length + 1).
////                                
////                int i := 0.
////                
////                while (i < length)
////                [
////                    retVal setField int:i object(self getField int:i).
////                    
////                    i := i + 1
////                ].
////                
////                retVal setField int:length object:object.
////                
////                ^ retVal
////            ];
////            [ 
////                if(object class_ref == Cast)
////                    [
////                        int length := object fieldCount.
////                    
////                        var retVal := Cast $new int:(length + 1).
////                                        
////                        int i := 0.
////                        
////                        while (i < length)
////                        [
////                            retVal setField int(i + 1) object(self getField int:i).
////                            
////                            i := i + 1
////                        ].
////                        
////                        retVal setField int:0 object:object.
////                        
////                        ^ retVal
////                    ];
////                    [ ^ Cast new args:(self,object) ]
////            ].
////    ]        
//}
