import system'collections;

public class Task
{
   TaskStatus          _status;
   Exception?          _exception;
   List<Action<Task>>  _continuations;
   Func                _action;

   protected constructor()
   {
      initialize()
   }

   protected initialize()
   {
      _status := TaskStatus.Created;
      _continuations := new List<Action<Task>>();
   }

   internal constructor newPromise()
   {
   }

   constructor assign(Func action)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      _action := action;
   }

   constructor assign(Func1 action, object arg)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      _action := new StartHelper(action, arg);
   }

   private bool isCompletedUnsafe()
      = _status == TaskStatus.RanToCompletion
         || _status == TaskStatus.Canceled
         || _status == TaskStatus.Faulted;

   TaskStatus Status
   {
      get()
      {
         TaskStatus status := TaskStatus.Created;
         lock(self) {
            status := _status
         };

         ^ status
      }
   }

   bool IsCompleted
   {
      get()
      {
         bool completed := false;
         lock(self) {
            completed := isCompletedUnsafe()
         };

         ^ completed
      }
   }

   bool IsCompletedSuccessfully
      = self.Status == TaskStatus.RanToCompletion;

   bool IsFaulted
      = self.Status == TaskStatus.Faulted;

   bool IsCanceled
      = self.Status == TaskStatus.Canceled;

   indexed get Result()
   {
      wait();

      ^ nil;
   }

   indexed internal getResultUnsafe()
      = nil;

   indexed internal Exception? Exception
   {
      get()
      {
         Exception? exception := nil;
         lock(self) {
            exception := _exception
         };

         ^ exception
      }
   }

   private registerContinuation(Action<Task> action)
   {
      bool scheduleNow := false;
      lock(self) {
         if (isCompletedUnsafe()) {
            scheduleNow := true
         }
         else {
            _continuations.append(action)
         };
      };

      if (scheduleNow)
         ThreadPool.queueAction(action, self);
   }

   // Used by the compiler generated async state machine. Unlike continueWith,
   // this does not allocate another Task merely to represent the callback.
   internal sealed onCompleted(Action<Task> action)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      registerContinuation(action)
   }

   sealed Task continueWith(Action<Task> action)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      Task continuationTask := Task.newPromise();
      registerContinuation(
         (Task antecedent)
         {
            try {
               action(antecedent);
               continuationTask.setResult();
            }
            catch (Exception e) {
               continuationTask.setException(e)
            }
         });

      ^ continuationTask
   }

   private bool complete(TaskStatus finalStatus, Exception? ex)
   {
      List<Action<Task>> continuations := new List<Action<Task>>();

      lock(self) {
         if (isCompletedUnsafe())
            { ^ false };

         _exception := ex;
         _status := finalStatus;
         continuations := _continuations;
         _continuations := new List<Action<Task>>();
      };

      int len := continuations.Length;
      for (int i := 0; i < len; i += 1) {
         Action<Task> continuation := continuations[i];
         ThreadPool.queueAction(continuation, self);
      };

      ^ true
   }

   internal sealed setResult()
   {
      if:not (complete(TaskStatus.RanToCompletion, nil))
         InvalidOperationException.raise("The task is already completed")
   }

   internal sealed bool trySetResult()
      = complete(TaskStatus.RanToCompletion, nil);

   internal sealed setException(Exception ex)
   {
      if (ex == nil)
         InvalidArgumentException.raise("ex");

      if:not (complete(TaskStatus.Faulted, ex))
         InvalidOperationException.raise("The task is already completed")
   }

   internal sealed bool trySetException(Exception ex)
   {
      if (ex == nil)
         InvalidArgumentException.raise("ex");

      ^ complete(TaskStatus.Faulted, ex)
   }

   sealed wait()
   {
      if:not (self.IsCompleted) {
         ManualResetEvent completed := ManualResetEvent.new();
         try {
            onCompleted((Task task){ completed.set() });
            completed.wait();
         }
         finally {
            completed.close()
         }
      };

      Exception? exception := self.Exception;
      if:not:nil(exception)
         exception.raise();

      ^ self
   }

   sealed start()
   {
      lock(self) {
         if (_status != TaskStatus.Created)
            InvalidOperationException.raise("The task can only be started once");

         _status := TaskStatus.Scheduled;
      };

      ThreadPool.queueAction(
      {
         lock(self) {
            _status := TaskStatus.Running
         };

         try {
            _action();
            setResult();
         }
         catch (Exception e) {
            trySetException(e);
         }
      });
   }

   static Task whenAllArgs(params Task[] tasks)
      = Task.whenAll(tasks);

   static Task whenAll(Task[] tasks)
   {
      if (tasks == nil)
         InvalidArgumentException.raise("tasks");

      Task promise := Task.newPromise();
      int remaining := tasks.Length;
      List<Exception> exceptions := new List<Exception>();

      if (remaining == 0) {
         promise.setResult();

         ^ promise
      };

      Action<Task> continuation := (Task completed)
      {
         bool last := false;
         lock(promise) {
            Exception? exception := completed.Exception;
            if:not:nil(exception)
               exceptions.append(exception);

            remaining := remaining - 1;
            last := remaining == 0;
         };

         if (last) {
            if (exceptions.Length > 0) {
               promise.setException(AggregateException.new(exceptions.Value))
            }
            else {
               promise.setResult()
            };
         }
      };

      int len := tasks.Length;
      for (int i := 0; i < len; i += 1) {
         Task task := tasks[i];
         if (task == nil)
            InvalidArgumentException.raise("tasks");

         task.onCompleted(continuation);
      };

      ^ promise
   }

   static waitAll(Task[] tasks)
      = Task.whenAll(tasks).wait();

   static waitAllArgs(params Task[] tasks)
      = Task.whenAll(tasks).wait();

   static Task run(Func action)
   {
      Task task := Task.assign(action);
      task.start();

      ^ task
   }

   static Task run(Func1 action, object arg)
   {
      Task task := Task.assign(action, arg);
      task.start();

      ^ task
   }

   static Task sleep(int msecs)
   {
      if (msecs < 0)
         InvalidArgumentException.raise("msecs");

      ^ Task.run({ Thread.sleep(msecs) })
   }

   static Task completedTask()
   {
      Task task := Task.newPromise();
      task.setResult();

      ^ task
   }

   static Task fromException(Exception ex)
   {
      Task task := Task.newPromise();
      task.setException(ex);

      ^ task
   }
}

// --- Task<TResult> ---

public sealed class Task<TResult> : Task
{
   TResult _result;

   protected constructor()
   {
      initialize()
   }

   constructor(TResult retVal)
   {
      initialize();
      _result := retVal;
      setResult();
   }

   internal constructor newPromise()
   {
      initialize()
   }

   constructor assign(Func<TResult> func)
   {
      initialize();

      if (func == nil)
         InvalidArgumentException.raise("func");

      _action :=
      {
         TResult retVal := func();
         setResultValue(retVal);
      };
   }

   indexed internal setResultValue(TResult result)
   {
      lock(self) {
         _result := result
      }
   }

   indexed internal TResult getResultUnsafe()
      = _result;

   indexed get TResult Result()
   {
      wait();

      ^ _result
   }

   TResult cast()
      = self.Result;

   static Task<TResult> run(Func<TResult> action)
   {
      Task<TResult> task := class Task<TResult>.assign(action);
      task.start();

      ^ task
   }

   static Task<TResult> fromResult(TResult result)
      = new Task<TResult>(result);
}
