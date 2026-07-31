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

   wait(int timeOut)
   {
      excluded
      {
         extern KERNEL32.WaitForSingleObject(_handle, timeOut);
      }      
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

   // every call below blocks in libc, so it has to run inside a safe region : a thread
   // stopped in pthread_mutex_lock has no safe point ahead of it, and the collector
   // would add it to the wait list and then wait for a signal that never comes. The
   // Windows branch above already wraps all four
   // the negative range is the auto reset mode, 80000000h is armed and 80000001h is
   // signalled. The old branch waited once with no predicate and no consumption, so a
   // spurious wakeup released the waiter and a set before the wait was lost entirely.
   // Windows semantics : the waiter that leaves consumes the signal, exactly one leaves
   wait()
   {
      // only the blocking libc calls belong in a safe region. A collection skips a thread
      // that is inside one, so ELENA code must not run there : the field accesses below
      // would go on through a frozen frame while the collector moves the object
      excluded { extern libc.pthread_mutex_lock(_mutex) };

      if (_signaled < 0) {
         while (_signaled == 80000000h) {
            excluded { extern libc.pthread_cond_wait(_cond, _mutex) }
         };

         _signaled := 80000000h
      }
      else {
         while (_signaled == 0) {
            excluded { extern libc.pthread_cond_wait(_cond, _mutex) }
         }
      };

      excluded { extern libc.pthread_mutex_unlock(_mutex) }
   }

   wait(int timeOut)
   {
      NotSupportedException.raise()
   }

   set()
   {
      excluded { extern libc.pthread_mutex_lock(_mutex) };

      if (_signaled < 0) {
         // one token at most, and one waiter wakes to consume it : signal, not
         // broadcast, the consume loop in wait covers a spurious extra wakeup
         if (_signaled == 80000000h) {
            _signaled := 80000001h;

            excluded { extern libc.pthread_cond_signal(_cond) }
         }
      }
      else {
         if (_signaled != 1) {
            if (_signaled == 0)
               _signaled := 1;

            excluded { extern libc.pthread_cond_broadcast(_cond) }
         }
      };

      excluded { extern libc.pthread_mutex_unlock(_mutex) }
   }

   reset()
   {
      excluded { extern libc.pthread_mutex_lock(_mutex) };

      // the old code stored a plain zero, which silently turned an auto reset event
      // into a manual one
      if (_signaled < 0) {
         _signaled := 80000000h
      }
      else {
         _signaled := 0
      };

      excluded { extern libc.pthread_mutex_unlock(_mutex) }
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

   wait(int timeOut)
      => _handle;

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
      _handle := EventHandle.new(false, false);
      _counter := counter;
   }

   wait()
      => _handle;

   wait(int timeOut)
      => _handle;

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
         { _handle.set() }
   }

   close()
      => _handle;
}
