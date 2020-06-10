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
    
//    dispatch() => intern system'core_routines'__group;
}

// --- extensions ---

public extension dynamicOp
{
    mixInto(role)
        = new Extension(self, role);
        
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
}
