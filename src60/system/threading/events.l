import system;

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

public sealed struct CountDownEvent
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

#endif