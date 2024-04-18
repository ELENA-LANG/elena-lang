
// --- Message ---
public sealed const struct Message
   : info("A message constant")
{
   embeddable __mssg value[4];

   private load(string name) : external(system'core_routines'__message_load);
  
   static Message Default
      = default;

   constructor(string name)
   {
      self.load(name);
   }

   bool equal(Message mssg)
      = value == mssg;

   MessageName MessageName
      = MessageName.load(self);

   string toPrintable()
   {
      byte buffer[256];
      int len := extern LoadMessageNameLA(value, buffer, 255);

      ^ String.fromByteArray(0, len, buffer);
   }

   dispatch() : external(system'core_routines'__message_dispatcher);
}

public sealed const struct ExtensionMessage
   : info("An extended message constant")
{
   embeddable __ext_mssg value;
    
//   bool equal(ExtensionMessage ext_mssg)
//         : info("Compares the values")
//      = value == ext_mssg;

   dispatch() : info("Dequalifies incoming messages if they are equal to its value and redirects to the target"), 
                  external(system'core_routines'__ext_message_dispatcher);
}


// --- MessageName ---
public sealed const struct MessageName
{
   embeddable __subj value[4];

   private loadMessageName(Message mssg) : external(system'core_routines'__mssgname_load);

   private load(string name) : external(system'core_routines'__messagename_loadbystr);

   private createGetProperty(ref Message mssg) : external(system'core_routines'__mssgname_getprop);

   private createSetProperty(ref Message mssg) : external(system'core_routines'__mssgname_setprop);

   private createVariadic(ref Message mssg) : external(system'core_routines'__mssgname_variadic);

   static MessageName Default
      = default;

   /// Creates a dynamic signature
   constructor(string s)
   {
      self.load(s);
   }

   internal constructor load(Message mssg)
   {
      self.loadMessageName(mssg);
   }

   bool equal(MessageName subject)
      = value == subject;

   Message getPropertyMessage()
   {
      self.createGetProperty(ref Message message);
        
      ^ message
   }

   Message setPropertyMessage()
   {
      self.createSetProperty(ref Message message);
        
      ^ message
   }

   get Message VariadicMessage()
   {
      self.createVariadic(ref Message message);
        
      ^ message        
   }

   string toPrintable()
   {
      byte buffer[256];
      int  length := extern LoadActionNameLA(self, buffer, 255);
        
      ^ String.fromByteArray(0, length, buffer)
   }

   dispatch() : external(system'core_routines'__mssgname_dispatcher),
      info("Qualifies incoming messages and redirects to the target");
}

