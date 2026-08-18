#if (__project["_Linux32"])

internal const int __SIZEOF_SEM_T = 16;

#elif (__project["_MacOSARM64"])

internal const int __SIZEOF_SEM_T = 4;

#elif (__project["_Linux64"] || __project["_LinuxPPC64le"] || __project["_LinuxARM64"] || __project["_FreeBSD64"])

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
      excluded
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

public struct Semaphore
{
   pointer _sem;

   constructor create(int initValue)
   {
      _sem := extern libc.malloc(__SIZEOF_SEM_T);

      extern libc.sem_init(_sem, 0, initValue);
   }

   close()
   {
      extern libc.sem_destroy(_sem);

      extern libc.free(_sem);
   }

   wait()
   {
      excluded
      {
         while (extern libc.sem_wait(_sem) != 0) {
            // sem_wait is interrupted by signals on POSIX. Retry instead of
            // consuming an item that was never acquired.
         }
      }      
   }   

   release()
   {
      extern libc.sem_post(_sem);
   }
}

#elif (__project["_MacOS"])

public struct Semaphore
{
   pointer _sem;

   constructor create(int initValue)
   {
      _sem := extern libc.dispatch_semaphore_create(initValue);
   }

   close()
   {
   }

   wait()
   {
      excluded
      {
         extern libc.dispatch_semaphore_wait(_sem, -1l);
      }
   }

   release()
   {
      extern libc.dispatch_semaphore_signal(_sem);
   }
}

#endif
