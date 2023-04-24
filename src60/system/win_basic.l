// --- Handle ---

public sealed const struct Handle
{
   embeddable __ptr value;

   constructor(pointer ptr)
   {
      value := ptr
   }

   bool isUnassigned()
   {
      ^ value == 0;
   }
}

// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
   embeddable __ptr pointer;

   private readIntPtr(ref int retVal) : external(system'core_routines'__readIntPtr);

   bool isUnassigned()
   {
      ^ pointer == 0;
   }

   // unsafe code
   string cast()
   {
      int  len := extern KERNEL32.lstrlenA(self);
      string s := String.allocate(len);
      extern KERNEL32.lstrcpyA(s, self);
        
      ^ s
   }

   wide cast()
   {
      int  len := extern KERNEL32.lstrlenW(self);
      wide s := WideString.allocate(len);
      extern KERNEL32.lstrcpyW(s, self);
        
      ^ s
   }

   int cast()
   {
      self.readIntPtr(ref int retVal);

      ^ retVal
   }

   copyTo(byte[] target, int len) : external(system'core_routines'__ptrCopyTo);
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

   string toPrintable()
      : info("Generates the call stack literal representation")
   {
      byte    buffer[512];
      int     bufferLength := 0; 
      pointer address;
      string  callStackText := emptyString;
      int     length := stack.Length;
      for (int i := 1, i < length, i += 1) {
         address := stack[i];

         bufferLength := extern LoadAddressInfoLM(address, buffer, 512);
         if (bufferLength > 0) {
            callStackText := callStackText + String.fromByteArray(0, bufferLength, buffer).add(newLineConstant)
         }
      };

      ^ callStackText
   }
}
