#if (__project["_Win"]) // Windows

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

#elif (__project["_Linux"] || __project["_FreeBSD"])

internal struct pthread_mutex_t
{
   byte __size[__SIZEOF_PTHREAD_MUTEX_T];
}

internal struct pthread_cond_t
{
   byte __size[__SIZEOF_PTHREAD_COND_T];
}

internal struct EventHandle
{
   int             _signaled;
   pthread_mutex_t _mutex;
   pthread_cond_t  _cond;

   internal constructor new(bool value, bool autoReset)
   {
      pthread_mutex_t mutex;
      extern libc.pthread_mutex_init(mutex, 0);
      _mutex := mutex;

      pthread_cond_t cond;
      extern libc.pthread_cond_init(cond, 0);
      _cond := cond;
      
      _signaled := value ? 1 : 0;
      if (autoReset)
         _signaled := _signaled | 80000000h;
   }

   wait()
   {
      pthread_mutex_t mutex := _mutex;
      pthread_cond_t  cond := _cond;

      extern libc.pthread_mutex_lock(mutex);
      _mutex := mutex;

      if (_signaled < 0) {
         extern
         {
             extern libc.pthread_cond_wait(cond, mutex);
         }
      }
      else {
         extern
         {
            while (_signaled == 0) {
               extern libc.pthread_cond_wait(cond, mutex);
            }
         }
      };

      extern libc.pthread_mutex_unlock(mutex);
      _mutex := mutex;
   }

   wait(int timeOut)
   {
      NotSupportedException.raise()
   }

   set()
   {
      pthread_mutex_t mutex := _mutex;

      extern libc.pthread_mutex_lock(mutex);
      _mutex := mutex;

      if (_signaled == 0)
         _signaled := 1;

      extern libc.pthread_mutex_unlock(mutex);
      _mutex := mutex;

      pthread_cond_t  cond := _cond;
      extern libc.pthread_cond_broadcast(cond);
      _cond := cond;
   }

   reset()
   {
      if (_signaled == 1) {
         pthread_mutex_t mutex := _mutex;

         extern libc.pthread_mutex_lock(mutex);
         _mutex := mutex;

         _signaled := 0;

         extern libc.pthread_mutex_unlock(mutex);
         _mutex := mutex;
      }
   }

   close()
   {
      pthread_mutex_t mutex := _mutex;
      pthread_cond_t  cond := _cond;

      extern libc.pthread_mutex_destroy(mutex);
      extern libc.pthread_cond_destroy(cond);

      _mutex := mutex;
      _cond := cond;
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
      lock(self)
      {
         if (_counter > 0) {
            _counter--;

            if (_counter == 0)
              _handle.set() 
         }
      }
   }  

   close()
      => _handle;
}
