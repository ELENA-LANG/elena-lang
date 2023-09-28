// --- Handle ---

public sealed const struct Handle
{
   embeddable __ptr _value;

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

   get constructor Default()
      = new UnsafePointer();   

   private readIntPtr(ref int retVal) : external(system'core_routines'__readIntPtr);

   assignPtr(wide str) : external(system'core_routines'__assignPtr); 

   bool isUnassigned()
   {
      ^ _pointer == 0;
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
