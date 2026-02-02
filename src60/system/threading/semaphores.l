#if (__project["_Linux32"])

internal const int __SIZEOF_SEM_T = 16;

#elif (__project["_Linux64"])

internal const int __SIZEOF_SEM_T = 32;

#endif

#if (__project["_Win"]) // Windows

public struct Semaphore
{
   Handle  _handle;

   constructor create(int initValue, int maxValue)
   {
      _handle := extern KERNEL32.CreateSemaphoreA(0, initValue, maxValue, 0);
   }

   constructor create(int initValue)
      <= create(initValue, IntNumber.MaxValue);

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

#elif (__project["_Linux"] || __project["_FreeBSD"])

internal struct sem_t
{
   byte __size[__SIZEOF_SEM_T];
}

public struct Semaphore
{
   sem_t _sem;

   constructor create(int initValue)
   {
      extern libc.sem_init(_sem, 0, initValue);
   }

   close()
   {
      extern libc.sem_destroy(_sem);
   }

   wait()
   {
      extern
      {
         extern libc.sem_wait(_sem);
      }      
   }   

   release()
   {
      extern libc.sem_post(_sem);
   }
}

#endif
