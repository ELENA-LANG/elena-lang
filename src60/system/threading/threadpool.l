import system'collections'threadsafe;
import system'runtime;

public sealed class ThreadPool
{
   private constructor() {}

   static object              _sync := new object();
   static BlockingQueue<Func> _actions := new BlockingQueue<Func>();
   static bool                _initialized;
   static int                 _minThreads;
   static int                 _maxThreads;
   static int                 _workerCount;
   static int                 _idleCount;
   static int                 _pendingCount;

   static constructor()
   {
      int cpuCount := Environment.ProcessorCount;
      if (cpuCount < 1)
         cpuCount := 1;

      _initialized := false;
      _minThreads := 1;
      _maxThreads := cpuCount * 4;
      if (_maxThreads < _minThreads)
         _maxThreads := _minThreads;

      _workerCount := 0;
      _idleCount := 0;
      _pendingCount := 0;
   }

   // Configuration is deliberately explicit until linker-provided pool settings
   // are carried in SystemEnv. It must be called before the first queued action.
   static configure(int minThreads, int maxThreads)
   {
      if (minThreads < 1)
         InvalidArgumentException.raise("minThreads");

      if (maxThreads < minThreads)
         InvalidArgumentException.raise("maxThreads");

      lock(_sync) {
         if (_initialized)
            InvalidOperationException.raise("The thread pool is already initialized");

         _minThreads := minThreads;
         _maxThreads := maxThreads;
      }
   }

   private static workerLoop()
   {
      while (true) {
         lock(_sync) {
            _idleCount := _idleCount + 1
         };

         Func action := _actions.pop();

         lock(_sync) {
            _idleCount := _idleCount - 1;
            _pendingCount := _pendingCount - 1;
         };

         try {
            action()
         }
         catch (Exception e) {
            // A raw work item must not permanently remove a pool worker. Task
            // actions and continuations catch and store their own exceptions.
            Console.writeLine(e)
         }
      }
   }

   private static startWorker()
   {
      Thread.start({ workerLoop() });
   }

   private static growIfRequired()
   {
      int startCount := 0;
      lock(_sync) {
         if:not (_initialized) {
            _initialized := true;
            // The configured minimum is created only when the first work item
            // arrives. Further submissions grow toward the bounded maximum.
            startCount := _minThreads;
         }
         else if (_pendingCount > _idleCount && _workerCount < _maxThreads) {
            startCount := 1
         };

         int available := _maxThreads - _workerCount;
         if (startCount > available)
            startCount := available;

         _workerCount := _workerCount + startCount;
      };

      for (int i := 0; i < startCount; i += 1)
         startWorker();
   }

   static int WorkerCount
   {
      get()
      {
         int count := 0;
         lock(_sync) {
            count := _workerCount
         };

         ^ count
      }
   }

   static queueAction(Func action)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      lock(_sync) {
         _pendingCount := _pendingCount + 1
      };

      _actions.push(action);
      growIfRequired();
   }

   static queueAction(Func1 action, object arg)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      queueAction(new StartHelper(action, arg));
   }
}
