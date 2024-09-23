import system;

internal struct EventHandle
{
   Handle  _handle;

   internal constructor new(bool value, bool autoReset)
   {
      _handle := extern KERNEL32.CreateEventA(0, 
         value ? 1 : 0,
         autoReset ? 1 : 0,
         0);
   }

   wait()
   {
      extern
      {
         extern KERNEL32.WaitForSingleObject(_handle, INFINITE);
      }      
   }   

   wait(int timeOut)
   {
      extern
      {
         extern KERNEL32.WaitForSingleObject(_handle, timeOut);
      }      
   }   

   set()
   {
      extern
      {
         extern KERNEL32.SetEvent(_handle);
      }      
   }

   reset()
   {
      extern
      {
         extern KERNEL32.ResetEvent(_handle);
      }      
   }

   close()
   {
      extern KERNEL32.CloseHandle(_handle)
   }
}