import system'runtime;

internal sealed class TimerRegistration : interface<Func>
{
   long              _dueTime;
   Func?             _action;
   bool              _active;
   // Queued work can still be canceled until a pool worker begins it
   bool              _queued;
   bool              _executing;
   Thread?           _executingThread;
   ManualResetEvent? _completion;
   int               _waiterCount;
   int               _queueIndex;

   internal constructor new(long dueTime, Func action)
   {
      _dueTime := dueTime;
      _action := action;
      _active := true;
      _queued := false;
      _executing := false;
      _executingThread := nil;
      _waiterCount := 0;
      _queueIndex := -1
   }

   internal long DueTime
      = _dueTime;

   internal int QueueIndex
   {
      get()
         = _queueIndex;

      set(int index)
      {
         _queueIndex := index
      }
   }

   private completeExecution()
   {
      ManualResetEvent? completion := nil;
      lock(self) {
         _executing := false;
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
            _executing := true;
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

         if:not (_executing) {
            _queued := false;
            _action := nil;
         };

         if (_executing) {
            Thread? executingThread := _executingThread;
            if:nil(executingThread)
               InvalidOperationException.raise();
            if:not:nil(executingThread) {
               if:not (executingThread.equalReference(Thread.Current)) {
                  if (_completion == nil)
                     _completion := ManualResetEvent.new();

                  _waiterCount := _waiterCount + 1;
                  completion := _completion
               }
            }
         }
      };

      TimerQueue.remove(self);

      if:not:nil(completion) {
         completion!.wait();

         bool closeCompletion := false;
         lock(self) {
            _waiterCount := _waiterCount - 1;
            if (_waiterCount == 0) {
               _completion := nil;
               closeCompletion := true
            }
         };

         if (closeCompletion)
            completion!.close()
      }
   }
}

internal sealed class TimerQueue
{
   private constructor() {}

   // Queue indices keep arbitrary timer removal logarithmic
   static object          _sync := new object();
   static Array?          _items;
   static Reference<int>  _count := 0;
   static AutoResetEvent? _wake;
   static bool            _started := false;

   private static ensureCapacity()
   {
      int count := *_count;
      if (_items == nil) {
         _items := Array.allocate(32)
      }
      else {
         Array items := _items!;
         if (count == items.Length) {
            Array expanded := Array.allocate(count * 2);
            for (int i := 0; i < count; i += 1)
               expanded[i] := items[i];

            _items := expanded
         }
      }
   }

   private static swap(Array items, int leftIndex, int rightIndex)
   {
      TimerRegistration left := items[leftIndex];
      TimerRegistration right := items[rightIndex];
      items[leftIndex] := right;
      items[rightIndex] := left;
      right.QueueIndex := leftIndex;
      left.QueueIndex := rightIndex
   }

   private static shiftUp(Array items, int index)
   {
      int current := index;
      while (current > 0) {
         int parent := (current - 1) / 2;
         TimerRegistration currentItem := items[current];
         TimerRegistration parentItem := items[parent];
         if (parentItem.DueTime <= currentItem.DueTime)
            { :break };

         swap(items, current, parent);
         current := parent
      }
   }

   private static shiftDown(Array items, int index)
   {
      int current := index;
      int count := *_count;
      while (true) {
         int left := current * 2 + 1;
         if (left >= count)
            { :break };

         int right := left + 1;
         int earliest := left;
         if (right < count) {
            TimerRegistration leftItem := items[left];
            TimerRegistration rightItem := items[right];
            if (rightItem.DueTime < leftItem.DueTime)
               earliest := right
         };

         TimerRegistration currentItem := items[current];
         TimerRegistration earliestItem := items[earliest];
         if (currentItem.DueTime <= earliestItem.DueTime)
            { :break };

         swap(items, current, earliest);
         current := earliest
      }
   }

   private static insert(TimerRegistration item)
   {
      ensureCapacity();

      Array items := _items!;
      int index := *_count;
      items[index] := item;
      item.QueueIndex := index;
      _count.append(1);
      shiftUp(items, index)
   }

   private static removeAt(int index)
   {
      Array items := _items!;
      int lastIndex := *_count - 1;
      TimerRegistration removed := items[index];
      removed.QueueIndex := -1;

      if (index == lastIndex) {
         items[lastIndex] := default;
         _count.reduce(1)
      }
      else {
         TimerRegistration replacement := items[lastIndex];
         items[index] := replacement;
         items[lastIndex] := default;
         replacement.QueueIndex := index;
         _count.reduce(1);

         bool movedUp := false;
         if (index > 0) {
            int parent := (index - 1) / 2;
            TimerRegistration parentItem := items[parent];
            if (replacement.DueTime < parentItem.DueTime) {
               shiftUp(items, index);
               movedUp := true
            }
         };

         if:not (movedUp)
            shiftDown(items, index)
      }
   }

   internal static remove(TimerRegistration item)
   {
      AutoResetEvent? wake := nil;
      lock(_sync) {
         Array items := _items!;
         int count := *_count;
         int index := item.QueueIndex;

         if (index >= 0 && index < count && items[index].equalReference(item)) {
            if (index == 0)
               wake := _wake;

            removeAt(index)
         }
      };

      if:not:nil(wake)
         wake!.set()
   }

   private static loop()
   {
      while (true) {
         TimerRegistration? expired := nil;
         AutoResetEvent? wake := nil;
         int waitTime := -1;
         lock(_sync) {
            wake := _wake;
            if (*_count > 0) {
               Array items := _items!;
               TimerRegistration first := items[0];
               long remaining := first.DueTime - Environment.TickCount64;
               if (remaining <= 0l) {
                  expired := first;
                  removeAt(0)
               }
               else {
                  waitTime := cast int(remaining)
               }
            }
         };

         if:not:nil(expired) {
            expired!.queueCallback()
         }
         else {
            wake!.wait(waitTime)
         }
      }
   }

   internal static TimerRegistration schedule(int milliseconds, Func action)
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

         insert(item);
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
