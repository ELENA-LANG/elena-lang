//import system'collections;

// --- classOp ---

public extension classROp
{
    setField(int index, object object)
        => intern system'core_routines'__object;
        
//    getField(int index)
//        => intern system'core_routines'__object;
//        
//    get int __FieldCount()
//    {
//        intern system'core_routines'__object.readLength(self, ref int retVal);
//        
//        ^ retVal
//    }
//        
//    bool respondTo(Message m)
//        => intern system'core_routines'__object;
//
//    Message[] __getMessages()
//    {
//        dirty_ptr class := self/*.__getClass()*/;
//
//        int tmp_array[256];
//        int retVal := extern elenart.core_readvmt(0,class,256,tmp_array);
//
//        List<Message> list := new List<Message>();
//        for(int i := 1, i < retVal, i += 1)
//        {
//           ifnot (tmp_array[i].anyMask(60h))
//           {
//               Message message;
//               intern system'core_routines'__message.copy(message, tmp_array[i]);
//               
//               list.append(message);
//           };
//        };
//
//        ^ list.Value
//    }
//    
//    MessageName[] __getProperties()
//    {
//        dirty_ptr class := self/*.__getClass()*/;
//
//        int tmp_array[256];
//        int retVal := extern elenart.core_readvmt(0,class,256,tmp_array);
//
//        auto list := new List<MessageName>();
//        for(int i := 0, i < retVal, i += 1)
//        {
//           int id := tmp_array[i];
//           if (id.allMask(41h))
//           {
//               int count := id && 01Fh;
//               if (count == 1) {
//                   Message message;
//                   intern system'core_routines'__message.copy(message, tmp_array[i]);
//                   
//                   MessageName prop := message.MessageName;
//                   
//                   list.append(prop);
//               }
//           };
//        };
//
//        ^ list.Value
//    }
    
    setFields(params object[] objects)
    {
        int l := objects.Length;
        for (int i := 0, i < l, i += 1) {
            self.setField(i, objects[i])
        };
        
        ^ self
    }
}

//public extension classExOp
//{
//    __newProxy(target)
//        => intern system'core_routines'__object;
//        
//    __inheritProxy()
//        => intern system'core_routines'__object;
//}
//
//public extension MessageExOp : Message
//{
//    __getFirstSignatureMember()
//        => intern system'core_routines'__message;
//}
//
//
