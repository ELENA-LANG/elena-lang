import system;

public struct Semaphore
{
   Handle  _handle;

   constructor create(int initValue, int maxValue)
   {
      _handle := extern KERNEL32.CreateSemaphoreA(0, initValue, maxValue, 0);
   }

   close()
   {
      extern KERNEL32.CloseHandle(_handle)
   }

   wait()
   {
      extern
      {
         extern KERNEL32.WaitForSingleObject(_handle, INFINITE);
      }      
   }   

   release()
   {
      extern KERNEL32.ReleaseSemaphore(_handle, 1, 0);
   }
}