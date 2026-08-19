#if (__project["_Linux32"])

internal const int __SIZEOF_PTHREAD_MUTEX_T = 24;
internal const int __SIZEOF_PTHREAD_COND_T  = 48;

#elif (__project["_MacOSARM64"])

internal const int __SIZEOF_PTHREAD_MUTEX_T = 64;
internal const int __SIZEOF_PTHREAD_COND_T  = 48;

#elif (__project["_Linux64"] || __project["_LinuxPPC64le"] || __project["_LinuxARM64"] || __project["_FreeBSD64"])

internal const int __SIZEOF_PTHREAD_MUTEX_T = 40;
internal const int __SIZEOF_PTHREAD_COND_T  = 48;

#endif

#if (__project["_Win"]) // Windows

internal struct EventHandle
{
   Handle  _handle;

   internal constructor new(bool value, bool autoReset)
   {
      _handle := extern KERNEL32.CreateEventA(0, 
         autoReset ? 0 : 1,
         value ? 1 : 0,
         0);
   }

   wait()
   {
      excluded
      {
         extern KERNEL32.WaitForSingleObject(_handle, INFINITE);
      }      
   }   

   bool wait(int timeOut)
   {
      int retVal := 0;
      excluded
      {
         retVal := extern KERNEL32.WaitForSingleObject(_handle, timeOut);
      };

      ^ retVal == WAIT_OBJECT_0
   }   

   set()
   {
      excluded
      {
         extern KERNEL32.SetEvent(_handle);
      }      
   }

   reset()
   {
      excluded
      {
         extern KERNEL32.ResetEvent(_handle);
      }      
   }

   close()
   {
      extern KERNEL32.CloseHandle(_handle)
   }
}

#elif (__project["_Linux"] || __project["_FreeBSD"] || __project["_MacOS"])

internal struct EventHandle
{
   pointer _mutex;
   pointer _cond;
   int     _signaled;

   internal constructor new(bool value, bool autoReset)
   {
      _mutex := extern libc.malloc(__SIZEOF_PTHREAD_MUTEX_T);
      _cond := extern libc.malloc(__SIZEOF_PTHREAD_COND_T);

      extern libc.pthread_mutex_init(_mutex, 0);

      extern libc.pthread_cond_init(_cond, 0);

      _signaled := value ? 1 : 0;
      if (autoReset)
         _signaled := _signaled | 80000000h;
   }

   wait()
   {
      auto mutexCopy := _mutex;
      auto condCopy := _cond;

      excluded { extern libc.pthread_mutex_lock(mutexCopy) };

      excluded
      {
         while ((_signaled & 1) == 0) {
            extern libc.pthread_cond_wait(condCopy, mutexCopy)
         }
      };

      if (_signaled < 0)
         _signaled := 80000000h;

      extern libc.pthread_mutex_unlock(mutexCopy);
   }

   bool wait(int timeOut)
   {
      if (timeOut < 0) {
         self.wait();

         ^ true
      };

#if (__project["_Linux32"])
      int deadline[2];
#else
      long deadline[2];
#endif
      extern libc.clock_gettime(0, deadline);

      int seconds := timeOut / 1000;
      int milliseconds := timeOut - seconds * 1000;
      deadline[0] := deadline[0] + seconds;
      deadline[1] := deadline[1] + milliseconds * 1000000;
      if (deadline[1] >= 1000000000) {
         deadline[0] := deadline[0] + 1;
         deadline[1] := deadline[1] - 1000000000;
      };

      auto mutexCopy := _mutex;
      auto condCopy := _cond;

      excluded { extern libc.pthread_mutex_lock(mutexCopy) };

      int result := 0;
      excluded
      {
         while ((_signaled & 1) == 0 && result == 0) {
            result := extern libc.pthread_cond_timedwait(condCopy, mutexCopy, deadline)
         }
      };

      bool signaled := (_signaled & 1) != 0;
      if (signaled && _signaled < 0)
         _signaled := 80000000h;

      extern libc.pthread_mutex_unlock(mutexCopy);

      ^ signaled
   }

   set()
   {
      excluded { extern libc.pthread_mutex_lock(_mutex) };

      if ((_signaled & 1) == 0) {
         if (_signaled < 0) {
            _signaled := _signaled | 1;
            extern libc.pthread_cond_signal(_cond)
         }
         else {
            _signaled := 1;
            extern libc.pthread_cond_broadcast(_cond)
         };
      };

      extern libc.pthread_mutex_unlock(_mutex);

   }

   reset()
   {
      excluded { extern libc.pthread_mutex_lock(_mutex) };

      if (_signaled < 0) {
         _signaled := 80000000h
      }
      else {
         _signaled := 0
      };

      extern libc.pthread_mutex_unlock(_mutex);
   }

   close()
   {
      extern libc.pthread_mutex_destroy(_mutex);
      extern libc.pthread_cond_destroy(_cond);

      extern libc.free(_mutex);
      extern libc.free(_cond);
   }
}

#endif

public closed class BaseEvent
{
   EventHandle _handle;

   wait()
      => _handle;

   bool wait(int timeOut)
      = _handle.wait(timeOut);

   set()
      => _handle;

   reset()
      => _handle;

   close()
      => _handle;
}

public sealed class ManualResetEvent : BaseEvent
{
   constructor new()
      <= new(false)
   {
   }

   constructor new(bool value)
   {
      _handle := EventHandle.new(value, false);
   }
}

public sealed class AutoResetEvent : BaseEvent
{
   constructor new()
   {
      _handle := EventHandle.new(false, true);
   }
}

public sealed class CountDownEvent
{
   EventHandle _handle;
   int         _counter;

   constructor new(int counter)
   {
      if (counter < 0)
         InvalidArgumentException.raise("counter");

      _handle := EventHandle.new(counter == 0, false);
      _counter := counter;
   }

   wait()
      => _handle;

   bool wait(int timeOut)
      = _handle.wait(timeOut);

   signal()
   {
      // the counter needs the lock, set does not, and holding an ELENA spin lock across
      // a blocking libc call makes every other signaller spin on a CPU the holder needs.
      // Only the thread that reaches zero sets, so this stays single
      bool release := false;

      lock(self)
      {
         if (_counter > 0) {
            _counter--;

            release := (_counter == 0)
         }
      };

      if (release)
         _handle.set() 
   }  

   close()
      => _handle;
}
