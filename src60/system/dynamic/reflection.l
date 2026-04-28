import system'collections;

public class InvalidMockOperationException : InvalidOperationException
{
   constructor new()
      <= super new("Cannot mock an interface. Probably the executable contains no method output information");
}

public extension classROp
{
   Message[] __getMessages()
   {
      int tmp_array[256];
      int retVal := extern LoadClassMessagesLA(self, tmp_array, 0, 256);

      List<Message> list := new List<Message>();
      for(int i := 0; i < retVal; i += 1)
      {
         if:not (tmp_array[i].anyMask(60h))
         {
            Message message := MssgConvertor.convert(tmp_array[i]);
               
            list.append(message);
         };
      };

      ^ list.Value
   }
    
   MessageName[] __getProperties()
   {
      int tmp_array[256];
      int retVal := extern LoadClassMessagesLA(self, tmp_array, 0, 256);

      List<MessageName> list := new List<MessageName>();
      for(int i := 0; i < retVal; i += 1)
      {
         int id := tmp_array[i] & 0C0h;
         if (id == 080h)
         {
            int count := tmp_array[i] & 01Fh;
            if (count == 1) {
               Message message := MssgConvertor.convert(tmp_array[i]);
                   
               MessageName prop := message.MessageName;
                   
               list.append(prop);
            }
         };
      };

      ^ list.Value
   }

   bool respondTo(Message m)
   {
      int retVal := extern CheckClassMessageLA(self, m);

      ^ retVal != 0
   }

   bool respondTo(StrongMessage m)
   {
      int retVal := extern CheckClassMessageLA(self, m);

      ^ retVal != 0
   }

   ClassReference? __getMethodOutput(StrongMessage m)
   {
      pointer retVal := extern LoadMessageOutputLA(self, m);
      if (!retVal.isUnassigned()) {
          ^ ClassReference.loadUnsafe(retVal);
      };

      ^ nil;
   }

   ClassReference? __getMethodOutput(Message m)
   {
      pointer retVal := extern LoadMessageOutputLA(self, m);
      if (!retVal.isUnassigned()) {
          ^ ClassReference.loadUnsafe(retVal);
      };

      ^ nil;
   }

   __typecast(source) : external(system'core_routines'__type_cast); 
}

public extension signatureOp : Message
{
   int __loadSignature(object[] output, int maximalLength)
   {
      int retVal := extern LoadSignatureLA(self, output, maximalLength);

      ^ retVal
   }

   __getFirstSignatureMember()
   {
      object[] output := new object[](1);

      self.__loadSignature(output, 1);

      ^ output[0]
   }
}

extension reflectionOp
{
   __injectInferface(type)
     : external(system'core_routines'__createProxy);
}

public extension proxyTypeOp
{
   mockInferface(type)
   {
      var proxy := ::{ dispatch() => self; };

      ^ proxy.__injectInferface(type);
   }
}
