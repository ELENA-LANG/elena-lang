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
      self.assignPtr(obj);
   }

   get constructor Default()
      = new UnsafePointer();   

   private readIntPtr(ref int retVal) : external(system'core_routines'__readIntPtr);
   private readRealPtr(ref real retVal) : external(system'core_routines'__readRealPtr);

   private assignPtr(object obj) : external(system'core_routines'__assignPtr); 

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
      self.readIntPtr(ref int retVal);

      ^ retVal
   }

   real cast()
   {
      self.readRealPtr(ref real retVal);

      ^ retVal
   }

   copyTo(byte[] target, int len) : external(system'core_routines'__ptrCopyTo);
}
