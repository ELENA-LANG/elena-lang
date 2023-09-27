
// --- UnsafePointer ---

public sealed const struct UnsafePointer
{
   embeddable __ptr pointer;

   private clear() : external(system'core_routines'__clearPtr); 

   constructor()
   {
      self.clear();
   }

   get constructor Empty()
      = new UnsafePointer();   

   private readIntPtr(ref int retVal) : external(system'core_routines'__readIntPtr);

   assignPtr(wide str) : external(system'core_routines'__assignPtr); 

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
