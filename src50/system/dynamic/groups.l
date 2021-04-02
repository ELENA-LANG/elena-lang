//import system'text;
//
//// --- Group ---
//
//public mixin Group
//{
//    object[] items;
//    
//    internal constructor allocate(int len)
//        = new object[](len);
//        
//    constructor load(params object[] objects)
//        <= allocate(objects.Length)
//    {
//        self.setFields(params objects)
//    }
//    
//    dispatch() => intern system'core_routines'__group;
//}

// --- extensions ---

public extension dynamicOp
{
    mixInto(role)
        = new Extension(self, role);
        
//    groupWith(object)
//    {
//        if(nil == object)
//            { ^ self };
//        
//        if(Group == self.__getClass())
//        {
//            int length := self.__FieldCount;
//        
//            var retVal := Group.allocate(length + 1);
//                            
//            int i := 0;            
//            while (i < length)
//            {
//                retVal.setField(i, self.getField(i));
//                
//                i := i + 1
//            };
//            
//            retVal.setField(length, object);
//            
//            ^ retVal
//        }
//        else { 
//            if (Group == object.__getClass())
//            {
//                int length := object.__FieldCount;
//            
//                var retVal := Group.allocate(length + 1);
//                                
//                int i := 0;                
//                while (i < length)
//                {
//                    retVal.setField(i + 1, self.getField(i));
//                    
//                    i := i + 1
//                };
//                
//                retVal.setField(0, object);
//                
//                ^ retVal
//            }
//            else {
//                ^ Group.load(self,object)
//            }
//        }
//    }        
}

//public const struct ExtensionDispatcher
//{
//    embeddable __string dirty_ptr[] theArray;
//    
//    protected constructor allocate(int length)
//        = new dirty_ptr[](length);
//        
//    internal constructor copy(dirty_ptr[] array, int length)
//        <= allocate(length)
//    {
//        for(int i := 0 , i < length, i += 1)
//        {
//            theArray[i] := array[i]
//        }
//    }
//    
//    static ExtensionDispatcher load(Message message, string[] namespaces)
//    {
//        dirty_ptr output[64];
//        
//        byte tmp[1024];
//        int nsLen := namespaces.Length;
//        int tmpIndex := 0;
//        for(int i := 0, i < nsLen, i+=1) {
//            auto s := namespaces[i];
//            int sLen := s.Length;
//            
//            UTF8Encoding.convert(tmpIndex, sLen, s, tmp);
//            tmpIndex += sLen;
//            tmp[tmpIndex] := 0;
//            tmpIndex += 1;
//            
//            if (tmpIndex >= 1024){
//                InvalidArgumentException.raise()
//            }
//        };
//        tmp[tmpIndex] := 0;
//        
//        intern system'core_routines'__extdispatcher.load(output, message, tmp, ref int len);
//        if (len == 1)
//        {
//            ^ nil
//        };
//        
//        output[len] := 0;
//        len += 1; 
//        
//        ^ ExtensionDispatcher.copy(output, len)
//    }
//    
//    dispatch() => intern system'core_routines'__extdispatcher;    
//}
//
//
//public const struct VariadicExtensionDispatcher
//{
//    embeddable __string dirty_ptr[] theArray;
//    
//    protected constructor allocate(int length)
//        = new dirty_ptr[](length);
//        
//    internal constructor copy(dirty_ptr[] array, int length)
//        <= allocate(length)
//    {
//        for(int i := 0 , i < length, i += 1)
//        {
//            theArray[i] := array[i]
//        }
//    }
//    
//    static VariadicExtensionDispatcher load(Message message, string[] namespaces)
//    {
//        dirty_ptr output[64];
//        
//        byte tmp[1024];
//        int nsLen := namespaces.Length;
//        int tmpIndex := 0;
//        for(int i := 0, i < nsLen, i+=1) {
//            auto s := namespaces[i];
//            int sLen := s.Length;
//            
//            UTF8Encoding.convert(tmpIndex, sLen, s, tmp);
//            tmpIndex += sLen;
//            tmp[tmpIndex] := 0;
//            tmpIndex += 1;
//            
//            if (tmpIndex >= 1024){
//                InvalidArgumentException.raise()
//            }
//        };
//        tmp[tmpIndex] := 0;
//        
//        intern system'core_routines'__extdispatcher.load(output, message, tmp, ref int len);        
//        if (len == 1)
//        {
//            ^ nil
//        };
//        
//        output[len] := 0;
//        len += 1; 
//        
//        ^ VariadicExtensionDispatcher.copy(output, len)
//    }
//    
//    dispatch() => intern system'core_routines'__vextdispatcher;    
//}
