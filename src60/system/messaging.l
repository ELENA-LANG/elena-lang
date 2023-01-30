
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

// --- MessageName ---
public sealed const struct MessageName
{
   embeddable __subj value[4];

   private loadMessageName(Message mssg) : external(system'core_routines'__mssgname_load);

   internal constructor load(Message mssg)
   {
      self.loadMessageName(mssg);
   }
}
