// --- PrimitivePointerOperations ---
internal singleton PrimitivePointerOperations
{
   string readString(pointer ptr)
   {
      int  len := extern libc.strlen(ptr);
      string s := String.allocate(len);
      extern libc.strcpy(s, ptr);
        
      ^ s
   }

   wide readWideString(pointer ptr)
   {
      int  len := extern libc.wcslen(ptr);
      wide s := WideString.allocate(len);
      extern libc.wcscpy(s, ptr);
        
      ^ s
   }
}

// --- CallStack ---

public sealed class CallStack
   : info("A call stack")
{
   pointer[] stack;

   private load(pointer[] stack, int length, ref int retVal) : external(system'core_routines'__loadCallStack);

   constructor()
   {
      pointer stack[64];
      self.load(stack, 64, ref int length);

      this stack := class Array<pointer>.copy(stack, 0, length);
   }

   constructor(int trimingLevel)
   {
      pointer stack[64];
      self.load(stack, 64, ref int length);

      this stack := class Array<pointer>.copy(stack, trimingLevel, length - trimingLevel);
   }

   string toPrintable()
      : info("Generates the call stack literal representation")
   {
      byte    buffer[512];
      int     bufferLength := 0; 
      pointer address;
      string  callStackText := emptyString;
      int     length := stack.Length;
      for (int i := 1; i < length; i += 1) {
         address := stack[i];

         bufferLength := extern LoadAddressInfoLM(address, buffer, 512);
         if (bufferLength > 0) {
            callStackText := callStackText + String.fromByteArray(0, bufferLength, buffer).add(newLineConstant)
         };
      };

      ^ callStackText
   }
}
