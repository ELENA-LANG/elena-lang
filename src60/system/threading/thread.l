import system;

// --- Thread ---

public sealed class Thread
{
   ThreadHandle _handle;

   constructor assign(Func f)
   {
      _handle := new ThreadHandle(f);
   }

   constructor assign(Func f, int stackSize)
   {
      _handle := new ThreadHandle(f, stackSize);
   }

   ThreadPriority Priority
   {
      get()
         = _handle.getPriority();

      set(value)
      {
         _handle.setPriority(value);
      }
   }

   start() => _handle;

   join() => _handle;

   close() => _handle;
}