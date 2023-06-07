
// --- Message ---
public sealed const struct Message
{
   embeddable __mssg value[4];

   private load(string name) : external(system'core_routines'__message_load);
  
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
    
//    [info:"Compares the values"]
//    bool equal(ExtensionMessage ext_mssg)
//        = theValue == ext_mssg;

   dispatch() : info("Dequalifies incoming messages if they are equal to its value and redirects to the target"), 
                  external(system'core_routines'__ext_message_dispatcher);
}


// --- MessageName ---
public sealed const struct MessageName
{
   embeddable __subj value[4];

   private loadMessageName(Message mssg) : external(system'core_routines'__mssgname_load);

   private load(string name) : external(system'core_routines'__messagename_loadbystr);

   /// Creates a dynamic signature
   constructor(string s)
   {
      self.load(s);
   }

   internal constructor load(Message mssg)
   {
      self.loadMessageName(mssg);
   }

   dispatch() : external(system'core_routines'__mssgname_dispatcher);
}

