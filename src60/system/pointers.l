// --- Handle ---

public sealed const struct Handle : info("Handle value")
{
   embeddable __ptr _value;

   private setDirty(int value) : external(system'core_routines'__setHandleDirectly); 

   constructor loadDirty(int value) : info("Initializes the handler with the provided <b>value</b>")
   {
      self.setDirty(value);
   }

   private clear() : external(system'core_routines'__clearPtr); 

   get constructor Default() : info("Returns an empty handler")
      = new Handle();   

   constructor() : info("Creates an empty handler")
   {
      self.clear();
   }

   constructor(pointer ptr) : info("Creates a copy of <b>ptr</b>")
   {
      _value := ptr
   }

   bool isUnassigned() : info("Returns true if it is an empty handler<br/>Otherwise returns false")
   {
      ^ _value == 0;
   }
}

// --- UnsafePointer ---

public sealed const struct UnsafePointer : info("Unsafe weak pointer")
{
   embeddable __ptr _pointer;

   private clear() : external(system'core_routines'__clearPtr);
   private readIntPtr(out int retVal) : external(system'core_routines'__readIntPtr);
   private readPtrValue(out UnsafePointer retVal) : external(system'core_routines'__readPtrVal);
   private readIntPtrValue(out int retVal) : external(system'core_routines'__readIntPtrVal);
   private readRealPtrValue(out real retVal) : external(system'core_routines'__readRealPtrVal);
   private readLongPtrValue(out long retVal) : external(system'core_routines'__readLongPtrVal);
   private assignPtr(object obj) : external(system'core_routines'__assignPtr); 

   constructor() : info("Creates a null pointer")
   {
      self.clear();
   }

   constructor(pointer ptr) : info("Creates a copy of <b>ptr</b>")
   {
      _pointer := ptr
   }

   constructor(object obj) : info("Creates a pointer to <b>obj</b><br/>It is unsafe operation, because the object reference can be modified by GC routines")
   {
      if (obj is:pointer) {
         _pointer := cast UnsafePointer(weak obj)
      }
      else self.assignPtr(obj);
   }

   get constructor Default() : info("Returns a null-pointer")
      = new UnsafePointer();   

   bool isUnassigned() : info("Returns true if it is a null-pointer<br/>Otherwise returns false")
   {
      ^ _pointer == 0;
   }

   // unsafe code
   string cast() : info("Returns a UTF-8 string referred by the pointer<br/>It is unsafe operation")
   {
      ^ PrimitivePointerOperations.readString(self);
   }

   wide cast() : info("Returns a UTF-16 string referred by the pointer<br/>It is unsafe operation")
   {
      ^ PrimitivePointerOperations.readWideString(self);
   }

   int cast() : info("Returns a 32-bit integer referred by the pointer<br/>It is unsafe operation")
   {
      self.readIntPtr(out int retVal);

      ^ retVal
   }

   get pointer Value() : info("Returns a pointer (32 / 64 bit number depending on the platform) referred by the pointer<br/>It is unsafe operation")
   {
      self.readPtrValue(out pointer retVal);

      ^ retVal
   }

   get retoverload int Value() : info("Returns a 32-bit integer referred by the pointer<br/>It is unsafe operation")
   {   
      self.readIntPtrValue(out int retVal);

      ^ retVal
   }

   get retoverload real Value() : info("Returns a 64-bit float number referred by the pointer<br/>It is unsafe operation")
   {
      self.readRealPtrValue(out real retVal);

      ^ retVal
   }

   get retoverload long Value() : info("Returns a 64-bit integer referred by the pointer<br/>It is unsafe operation")
   {
      self.readLongPtrValue(out long retVal);

      ^ retVal
   }

   get retoverload string Value() : info("Returns a UTF-8 string referred by the pointer<br/>It is unsafe operation")
   {
      ^ PrimitivePointerOperations.readString(self);
   }

   get retoverload wide Value() : info("Returns a UTF-16 string referred by the pointer<br/>It is unsafe operation")
   {
      ^ PrimitivePointerOperations.readWideString(self);
   }

   long getLongValue()
   {
      long val := self.Value;

      ^ val;
   }
   
   string getStringUnsafe()
   {
      string str := self.Value;

      ^ str
   }

   copyToUnsafe(object ptr, int length) : external(system'core_routines'__ptrCopyTo), info("Copies the buffer referred by the pointer to <b>ptr</b> with provided <b>length</b>");

   copyTo(byte[] target, int len) : external(system'core_routines'__ptrCopyTo), info("Copies the buffer referred by the pointer to <b>target</b> byte array with provided <b>length</b>");
   copyTo(byte[] target, int index, int len) : external(system'core_routines'__ptrCopySubTo), info("Copies the buffer referred by the pointer to <b>target</b> byte sub array with provided <b>length</b> starting from <b>index</b>");
}

// --- UnsafePointer<T> ---

public sealed const struct UnsafePointer<T> : info("A strong-typed unsafe pointer")
{
   embeddable __ptr _pointer;

   private assignPtr(object obj) : external(system'core_routines'__assignPtr); 
   private loadData(T target, int size) : external(system'core_routines'__ptrCopyTo);

   constructor() : info("Creates a null pointer")
      = default;

   constructor(UnsafePointer<T> ptr) : info("Creates a copy of <b>ptr</b>")
   {
      _pointer := ptr
   }

   constructor(UnsafePointer ptr) : info("Creates a copy of weak <b>ptr</b> pointer")
   {
      _pointer := ptr
   }

   constructor(T obj) : info("Creates a pointer to <b>obj</b><br/>It is unsafe operation, because the object reference can be modified by GC routines")
   {
      self.assignPtr(obj);
   }

   get T Value() : info("Returns an ibstance of T type referred by the pointer<br/>It is unsafe operation")
   {
#if ($size class T > 0)

      T value := default;
      
      self.loadData(value, $size class T);
      
#elif ($size class T <= 0)
      T value := class T.Default;

      NotSupportedException.raise("Operation is not supported");

#endif

      ^ value
   }

   copyTo(T value, int size) : info("Copies the buffer referred by the pointer to <b>value</b> with provided <b>length</b><br/>NotSupportedException raises if T is not a structure")
   {
#if ($size class T >= 0)

      NotSupportedException.raise("Operation is not supported");
      
#elif ($size class T < 0)
      
      self.loadData(value, size);

#endif      
   }
}

// --- UnsafeArray ---

public sealed const struct UnsafeArray : info("A unsafe pointer to an array of pointers")
{
   embeddable __ptr _pointer;

   private clear() : external(system'core_routines'__clearPtr);
   
   constructor() : info("Creates a null pointer")
   {
      self.clear();
   }

   constructor(pointer ptr) : info("Creates a copy of weak <b>ptr</b> pointer")
   {
      _pointer := ptr
   }

   UnsafePointer at(int index) : info("Returns a pointer at the position <b>index</b>")
   {
      pointer item_ptr := _pointer + index * :sizeof pointer;
      
      ^ item_ptr
   }   
}

// --- UnsafeArray<T> ---

public sealed const struct UnsafeArray<T> : info("A unsafe pointer to a strong-typed array")
{
   embeddable __ptr _pointer;

   private clear() : external(system'core_routines'__clearPtr);
   
   constructor() : info("Creates a null pointer")
   {
      self.clear();
   }

   constructor(pointer ptr) : info("Creates a copy of weak <b>ptr</b> pointer")
   {
      _pointer := ptr
   }

   UnsafePointer<T> at(int index) : info("Returns an element of <b>T</b> type at the position <b>index</b>")
   {
      pointer item_ptr := _pointer + index * :sizeof pointer;
      
      ^ item_ptr
   }   
}

// --- CallStack ---

public sealed class CallStack
   : info("A call stack")
{
   pointer[] _stack;

   private load(pointer[] stack, int length, out int retVal) : external(system'core_routines'__loadCallStack);

   constructor() : info("Collects the call stack")
   {
      pointer stack[64];
      self.load(stack, 64, out int length);

      _stack := class Array<pointer>.copy(stack, 0, length);
   }

   constructor(int trimingLevel) : info("Collects the call stack skipping <b>trimingLevel</b>")
   {
      pointer stack[64];
      self.load(stack, 64, out int length);

      _stack := class Array<pointer>.copy(stack, trimingLevel, length - trimingLevel);
   }

   string toPrintable()
      : info("Generates the call stack literal representation")
   {
      byte    buffer[512];
      int     bufferLength := 0; 
      pointer address;
      string  callStackText := EmptyString;
      int     length := _stack.Length;
      for (int i := 1; i < length; i += 1) {
         address := _stack[i];

         bufferLength := extern LoadAddressInfoLM(address, buffer, 512);
         if (bufferLength > 0) {
            callStackText := callStackText + String.fromByteArray(0, bufferLength, buffer).add(NewLineConstant)
         }
      };

      ^ callStackText
   }   
}
