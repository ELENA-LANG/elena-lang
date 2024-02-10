import system;
import system'collections;

public extension classROp
{
   Message[] __getMessages()
   {
      int tmp_array[256];
      int retVal := extern LoadClassMessagesLA(self, tmp_array, 0, 256);

      List<Message> list := new List<Message>();
      for(int i := 0; i < retVal; i += 1)
      {
         ifnot (tmp_array[i].anyMask(60h))
         {
            Message message := mssgConvertor.convert(tmp_array[i]);
               
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
         if (id == 0C0h)
         {
            int count := tmp_array[i] & 01Fh;
            if (count == 1) {
               Message message := mssgConvertor.convert(tmp_array[i]);
                   
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
}