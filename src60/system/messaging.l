
// --- Message ---
public sealed const struct Message //: BaseValue
{
   embeddable __mssg value[4];

   string toPrintable()
   {
      byte buffer[256];
      int len := extern LoadMessageNameLA(value, buffer, 255);

      ^ String.fromByteArray(0, len, buffer);
   }

   dispatch() : external(system'core_routines'__message_dispatcher);
}