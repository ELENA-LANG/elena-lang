// --- Handle ---

public sealed const struct Handle
{
   embeddable __ptr _value;

   private setDirty(int value) : external(system'core_routines'__setHandleDirectly); 

   constructor loadDirty(int value)
   {
      self.setDirty(value);
   }

   private clear() : external(system'core_routines'__clearPtr); 

   get constructor Default()
      = new Handle();   

   constructor()
   {
      self.clear();
   }

   constructor(pointer ptr)
   {
      _value := ptr
   }

   bool isUnassigned()
   {
      ^ _value == 0;
   }
}

// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
   embeddable __ptr _pointer;

   private clear() : external(system'core_routines'__clearPtr);
   private readIntPtr(out int retVal) : external(system'core_routines'__readIntPtr);
   private readIntPtrValue(out int retVal) : external(system'core_routines'__readIntPtrVal);
   private readRealPtrValue(out real retVal) : external(system'core_routines'__readRealPtrVal);
   private assignPtr(object obj) : external(system'core_routines'__assignPtr); 

   constructor()
   {
      self.clear();
   }

   constructor(pointer ptr)
   {
      _pointer := ptr
   }

   constructor(object obj)
   {
      if (obj is: pointer) {
         _pointer := cast UnsafePointer(weak obj)
      }
      else self.assignPtr(obj);
   }

   get constructor Default()
      = new UnsafePointer();   

   bool isUnassigned()
   {
      ^ _pointer == 0;
   }

   // unsafe code
   string cast()
   {
      ^ PrimitivePointerOperations.readString(self);
   }

   wide cast()
   {
      ^ PrimitivePointerOperations.readWideString(self);
   }

   int cast()
   {
      self.readIntPtr(out int retVal);

      ^ retVal
   }

   get int Value()
   {   
      self.readIntPtrValue(out int retVal);

      ^ retVal
   }

   get retoverload real Value()
   {
      self.readRealPtrValue(out real retVal);

      ^ retVal
   }

   get retoverload string Value()
   {
      ^ PrimitivePointerOperations.readString(self);
   }

   get retoverload wide Value()
   {
      ^ PrimitivePointerOperations.readWideString(self);
   }

   copyTo(byte[] target, int len) : external(system'core_routines'__ptrCopyTo);
   copyTo(byte[] target, int index, int len) : external(system'core_routines'__ptrCopySubTo);
}

// --- CallStack ---

public sealed class CallStack
   : info("A call stack")
{
   pointer[] stack;

   private load(pointer[] stack, int length, out int retVal) : external(system'core_routines'__loadCallStack);

   constructor()
   {
      pointer stack[64];
      self.load(stack, 64, out int length);

      this stack := class Array<pointer>.copy(stack, 0, length);
   }

   constructor(int trimingLevel)
   {
      pointer stack[64];
      self.load(stack, 64, out int length);

      this stack := class Array<pointer>.copy(stack, trimingLevel, length - trimingLevel);
   }

   string toPrintable()
      : info("Generates the call stack literal representation")
   {
      byte    buffer[512];
      int     bufferLength := 0; 
      pointer address;
      string  callStackText := EmptyString;
      int     length := stack.Length;
      for (int i := 1; i < length; i += 1) {
         address := stack[i];

         bufferLength := extern LoadAddressInfoLM(address, buffer, 512);
         if (bufferLength > 0) {
            callStackText := callStackText + String.fromByteArray(0, bufferLength, buffer).add(NewLineConstant)
         }
      };

      ^ callStackText
   }
}
