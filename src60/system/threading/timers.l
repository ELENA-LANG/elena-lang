import system'collections;
import system'runtime;

internal sealed class TimerRegistration : interface<Func>
{
   long              _dueTime;
   Func?             _action;
   bool              _active;
   // Queued work can still be canceled until a pool worker begins it
   bool              _queued;
   Thread?           _executingThread;
   ManualResetEvent? _completion;
   Reference<int>    _waiterCount;

   internal constructor new(long dueTime, Func action)
   {
      _dueTime := dueTime;
      _action := action;
      _active := true;
      _queued := false;
      _executingThread := nil;
      _waiterCount := 0
   }

   internal long DueTime
      = _dueTime;

   private completeExecution()
   {
      ManualResetEvent? completion := nil;
      lock(self) {
         _executingThread := nil;
         completion := _completion
      };

      if:not:nil(completion)
         completion!.set()
   }

   private run()
   {
      Func? action := nil;
      lock(self) {
         if (_queued) {
            _queued := false;
            _executingThread := Thread.Current;
            action := _action;
            _action := nil
         }
      };

      if:not:nil(action) {
         try {
            action()
         }
         finally {
            completeExecution()
         }
      }
   }

   function()
   {
      run()
   }

   queueCallback()
   {
      bool execute := false;
      lock(self) {
         if (_active) {
            _active := false;
            _queued := true;
            execute := true
         }
      };

      if (execute)
         ThreadPool.queueAction(self)
   }

   close()
   {
      ManualResetEvent? completion := nil;
      lock(self) {
         _active := false;

         Thread? executingThread := _executingThread;
         if:nil(executingThread) {
            _queued := false;
            _action := nil;
         };

         if:not:nil(executingThread) {
            if:not (executingThread.equalReference(Thread.Current)) {
               if (_completion == nil)
                  _completion := ManualResetEvent.new();

               _waiterCount.append(1);
               completion := _completion
            }
         }
      };

      TimerQueue.remove(self);

      if:not:nil(completion) {
         completion!.wait();

         bool closeCompletion := false;
         lock(self) {
            _waiterCount.reduce(1);
            if (*_waiterCount == 0) {
               _completion := nil;
               closeCompletion := true
            }
         };

         if (closeCompletion)
            completion!.close()
      }
   }
}

internal singleton TimerQueue
{
   static object                         _sync := new object();
   static BinaryHeap<TimerRegistration>? _items;
   static AutoResetEvent?                _wake;
   static bool                           _started := false;

   private BinaryHeap<TimerRegistration> ensureQueue()
   {
      if (_items == nil)
         _items := BinaryHeap<TimerRegistration>.new((TimerRegistration left, TimerRegistration right => left.DueTime < right.DueTime));

      ^ _items!
   }

   internal remove(TimerRegistration item)
   {
      AutoResetEvent? wake := nil;
      lock(_sync) {
         if:not:nil(_items) {
            BinaryHeap<TimerRegistration> items := _items!;
            bool first := items.Length > 0 && items.peek().equalReference(item);
            if (items.remove(item) && first)
               wake := _wake
         }
      };

      if:not:nil(wake)
         wake!.set()
   }

   private loop()
   {
      while (true) {
         TimerRegistration? expired := nil;
         AutoResetEvent? wake := nil;
         int waitTime := -1;
         lock(_sync) {
            wake := _wake;
            BinaryHeap<TimerRegistration> items := _items!;
            if (items.Length > 0) {
               TimerRegistration first := items.peek();
               long remaining := first.DueTime - Environment.TickCount64;
               if (remaining <= 0l) {
                  expired := items.pop()
               }
               else {
                  waitTime := remaining as:int
               }
            }
         };

         if:not:nil(expired) {
            expired.queueCallback()
         }
         else {
            wake!.wait(waitTime)
         }
      }
   }

   internal TimerRegistration schedule(int milliseconds, Func action)
   {
      if (milliseconds < 0)
         InvalidArgumentException.raise("milliseconds");

      if (action == nil)
         InvalidArgumentException.raise("action");

      TimerRegistration item := TimerRegistration.new(Environment.TickCount64 + cast long(milliseconds), action);
      AutoResetEvent? wake := nil;
      bool start := false;
      lock(_sync) {
         if (_wake == nil)
            _wake := AutoResetEvent.new();

         ensureQueue().push(item);
         wake := _wake;
         if:not (_started) {
            _started := true;
            start := true
         }
      };

      if (start)
         Thread.start({ loop() });

      wake!.set();

      ^ item
   }
}
