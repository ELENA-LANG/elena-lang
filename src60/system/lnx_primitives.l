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
