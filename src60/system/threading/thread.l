import system;

// --- Thread ---

public sealed class Thread
{
   ThreadHandle _handle;

   constructor assign(Func f)
   {
      _handle := new ThreadHandle(f);
   }

   start() => _handle;

   join() => _handle;

   close() => _handle;
}