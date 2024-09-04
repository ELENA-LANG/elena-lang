import system;

// --- StartHelper ---

internal sealed class StartHelper : interface<Func>
{
   Func1  f;

   constructor(Func1 f)
   {
      this f := f
   }

   constructor(Func1 f, object arg)
   {
      this f := f;
      Argument := arg
   }

   object Argument : prop;

   function()
   {
      f(Argument)
   }
}

// --- Thread ---

public sealed class Thread
{
   __threadvar Thread _currentThread;

   ThreadHandle   _handle;
   object         _startArg;

   constructor assign(Func f)
   {
      _handle := new ThreadHandle(f);
   }

   constructor assign(Func f, int stackSize)
   {
      _handle := new ThreadHandle(f, stackSize);
   }

   constructor assign(Func1 f)
   {
      _startArg := new StartHelper(f);
      _handle := new ThreadHandle(_startArg);
   }

   constructor assign(Func1 f, int stackSize)
   {
      _startArg := new StartHelper(f);
      _handle := new ThreadHandle(_startArg, stackSize);
   }

   internal constructor current()
   {
      _handle := new ThreadHandle();
   }

   private static getCurrentThread()
   {
      _currentThread := Thread.current();

      ^ _currentThread
   }

   static Thread Current
   {
      get()
         = _currentThread ?? getCurrentThread();
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

   bool IsAlive
      = _handle.checkIfAlive();

   start()
   {
      if (handle.isNil())
         InvalidOperationException.raise();

      ifnot (_startArg.isNil())
         InvalidOperationException.raise();

      _handle.start();
   }

   start(object arg)
   {
      if (handle.isNil())
         InvalidOperationException.raise();

      if (_startArg.isNil())
         InvalidOperationException.raise();

      _startArg.Argument := arg;  

      _handle.start();
   }

   join() => _handle;

   join(int milliseconds) => _handle;

   close()
   {
      if (handle.isNil())
         InvalidOperationException.raise();

      _handle.close();

      _handle := nil;
   }

   static sleep(int milliseconds)
   {
      ThreadHandle.sleep(milliseconds)
   }
}