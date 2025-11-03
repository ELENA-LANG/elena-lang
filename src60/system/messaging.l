
// --- Message ---
public sealed const struct Message
   : info("A message constant")
{
   embeddable __mssg value[4];

   private load(string name) : external(system'core_routines'__message_load);
  
   static Message Default : info("Returns a default value")
      = default;

   constructor(string name) : info("Loads the message with the specified <b>name</b>")
   {
      self.load(name);
   }

   bool equal(Message mssg) : info("Returns true if the message is equal to <b>mssg</b>")
      = value == mssg;

   MessageName MessageName : info("Returns true if the message name")
      = MessageName.load(self);

   string toPrintable() : info("Returns a message literal constant")
   {
      byte buffer[256];
      int len := extern LoadMessageNameLA(value, buffer, 255);

      ^ String.fromByteArray(0, len, buffer);
   }

   dispatch() : info("Overrides the incomming message name with the message value and redirects to the target"),  
                  external(system'core_routines'__message_dispatcher);
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
   : info("A message name constant")
{
   embeddable __subj value[4];

   private loadMessageName(Message mssg) : external(system'core_routines'__mssgname_load);

   private load(string name) : external(system'core_routines'__messagename_loadbystr);

   private createGetProperty(ref Message mssg) : external(system'core_routines'__mssgname_getprop);

   private createSetProperty(ref Message mssg) : external(system'core_routines'__mssgname_setprop);

   private createVariadic(ref Message mssg) : external(system'core_routines'__mssgname_variadic);

   static MessageName Default : info("Returns a default value")
      = default;

   constructor(string s) : info("Creates a dynamic signature")
   {
      self.load(s);
   }

   internal constructor load(Message mssg)
   {
      self.loadMessageName(mssg);
   }

   bool equal(MessageName subject) : info("Returns true if the message name is equal to <b>subject</b>")
      = value == subject;

   Message getPropertyMessage() : info("Returns a property getter message")
   {
      self.createGetProperty(out Message message);
        
      ^ message
   }

   Message setPropertyMessage() : info("Returns a property setter message")
   {
      self.createSetProperty(out Message message);
        
      ^ message
   }

   get Message VariadicMessage() : info("Returns a variadic message")
   {
      self.createVariadic(out Message message);
        
      ^ message        
   }

   string toPrintable() : info("Returns a message name as a literal constant")
   {
      byte buffer[256];
      int  length := extern LoadActionNameLA(self, buffer, 255);
        
      ^ String.fromByteArray(0, length, buffer)
   }

   dispatch() : external(system'core_routines'__mssgname_dispatcher),
      info("Qualifies incoming messages and redirects to the target");
}

// --- PropertyMessageName ---
public sealed const struct PropertyMessageName : info("A property name constant")
{
   embeddable __subj value[4];

   private loadMessageName(Message mssg) : external(system'core_routines'__mssgname_load);

   private load(string name) : external(system'core_routines'__messagename_loadbystr);

   static MessageName Default : info("Returns a default value")
      = default;

   constructor(string s) : info("Creates a dynamic signature")
   {
      self.load(s);
   }

   internal constructor load(Message mssg) : info("Create a property name constant from the specified message <b>mssg</b>")
   {
      self.loadMessageName(mssg);
   }

   bool equal(MessageName subject) : info("Returns true if the message name is equal to <b>subject</b>")
      = value == subject;

   string toPrintable() : info("Returns a property name as a literal constant")
   {
      byte buffer[256];
      int  length := extern LoadActionNameLA(self, buffer, 255);
        
      ^ String.fromByteArray(0, length, buffer)
   }

   dispatch() : external(system'core_routines'__propname_dispatcher),
      info("Qualifies incoming messages and redirects to the target");
}
