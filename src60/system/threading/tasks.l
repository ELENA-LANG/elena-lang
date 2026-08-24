import system'collections;

public class Task
{
   TaskStatus          _status;
   Exception?          _exception;
   Action<Task>?       _continuation;
   List<Action<Task>>? _continuations;
   Func                _action;
   CancellationToken?  _cancellationToken;

   protected constructor()
   {
      initialize()
   }

   protected initialize()
   {
      _status := TaskStatus.Created;
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

   internal constructor assignCancelable(Func action, CancellationToken cancellationToken)
      <= assign(action)
   {
      _cancellationToken := cancellationToken
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

   indexed internal Exception? Exception = _exception;

   indexed internal CancellationToken cancellationToken
      = _cancellationToken ?? CancellationToken.None;

   private registerContinuation(Action<Task> action)
   {
      bool scheduleNow := false;
      lock(self) {
         if (isCompletedUnsafe()) {
            scheduleNow := true
         }
         else {
            if (_continuation == nil) {
               _continuation := action
            }
            else {
               if (_continuations == nil)
                  _continuations := new List<Action<Task>>();

               _continuations!.append(action)
            }
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

   private bool complete(TaskStatus finalStatus, Exception? ex, CancellationToken? cancellationToken)
   {
      Action<Task>? continuation := nil;
      List<Action<Task>>? continuations := nil;

      lock(self) {
         if (isCompletedUnsafe())
            { ^ false };

         _exception := ex;
         _cancellationToken := cancellationToken;
         _status := finalStatus;
         continuation := _continuation;
         continuations := _continuations;
         _continuation := nil;
         _continuations := nil;
      };

      if:not:nil(continuation)
         ThreadPool.queueAction(continuation, self);

      if:not:nil(continuations) {
         int len := continuations.Length;
         for (int i := 0; i < len; i += 1) {
            Action<Task> continuationItem := continuations[i];
            ThreadPool.queueAction(continuationItem, self);
         }
      };

      ^ true
   }

   internal sealed setResult()
   {
      if:not (complete(TaskStatus.RanToCompletion, nil, nil))
         InvalidOperationException.raise("The task is already completed")
   }

   internal sealed bool trySetResult()
      = complete(TaskStatus.RanToCompletion, nil, nil);

   internal sealed setException(Exception ex)
   {
      if (ex == nil)
         InvalidArgumentException.raise("ex");

      if:not (complete(TaskStatus.Faulted, ex, nil))
         InvalidOperationException.raise("The task is already completed")
   }

   internal sealed bool trySetException(Exception ex)
   {
      if (ex == nil)
         InvalidArgumentException.raise("ex");

      ^ complete(TaskStatus.Faulted, ex, nil)
   }

   internal sealed setCanceled(CancellationToken cancellationToken)
   {
      if (cancellationToken == nil)
         InvalidArgumentException.raise("cancellationToken");

      if:not (complete(TaskStatus.Canceled, nil, cancellationToken))
         InvalidOperationException.raise("The task is already completed")
   }

   internal sealed bool trySetCanceled(CancellationToken cancellationToken)
   {
      if (cancellationToken == nil)
         InvalidArgumentException.raise("cancellationToken");

      ^ complete(TaskStatus.Canceled, nil, cancellationToken)
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

      if (self.IsCanceled)
         TaskCanceledException.withToken(self.cancellationToken).raise();

      Exception? exception := self.Exception;
      if:not:nil(exception)
         exception.raise();

      ^ self
   }

   private executeAction()
   {
      CancellationToken cancellationToken := _cancellationToken ?? CancellationToken.None;
      bool hasCancellationToken := _cancellationToken != nil;

      if (hasCancellationToken && cancellationToken.IsCancellationRequested) {
         trySetCanceled(cancellationToken)
      }
      else {
         try
         {
            _action();
            setResult()
         }
         catch::
         {
            function(OperationCanceledException e)
            {
               if (hasCancellationToken
                  && cancellationToken.IsCancellationRequested
                  && cancellationToken.equalReference(e.Token))
               {
                  trySetCanceled(e.Token)
               }
               else trySetException(e)
            }

            function(Exception e)
            {
               trySetException(e)
            }
         }
      }
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

         executeAction()
      });
   }

   static Task whenAllArgs(params Task[] tasks)
      = Task.whenAll(tasks);

   static Task whenAll(Task[] tasks)
   {
      if (tasks == nil)
         InvalidArgumentException.raise("tasks");

      int length := tasks.Length;
      int remaining := length;
      for (int i := 0; i < length; i += 1) {
         if (tasks[i] == nil)
            InvalidArgumentException.raise("tasks")
      };

      Task promise := Task.newPromise();
      List<Exception>? exceptions := nil;
      CancellationToken? cancellationToken := nil;

      if (remaining == 0) {
         promise.setResult();

         ^ promise
      };

      Action<Task> continuation := (Task completed)
      {
         bool last := false;
         lock(promise) {
            if (completed.IsCanceled) {
               if (cancellationToken == nil)
                  cancellationToken := completed.cancellationToken
            }
            else {
               Exception? exception := completed.Exception;
               if:not:nil(exception) {
                  if (exceptions == nil)
                     exceptions := new List<Exception>();

                  exceptions!.append(exception)
               }
            };

            remaining := remaining - 1;
            last := remaining == 0;
         };

         if (last) {
            if (exceptions != nil) {
               promise.setException(AggregateException.new(exceptions!.Value))
            }
            else if (cancellationToken != nil) {
               promise.setCanceled(cancellationToken!)
            }
            else {
               promise.setResult()
            };
         }
      };

      for (int i := 0; i < length; i += 1)
         tasks[i].onCompleted(continuation);

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

   static Task run(Func action, CancellationToken token)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      if (token == nil)
         InvalidArgumentException.raise("token");

      if:not (token.CanBeCanceled)
         { ^ Task.run(action) };

      if (token.IsCancellationRequested)
         { ^ Task.fromCanceled(token) };

      Task task := Task.assignCancelable(action, token);
      task.start();

      ^ task
   }

   static Task sleep(int msecs)
   {
      if (msecs < -1)
         InvalidArgumentException.raise("msecs");

      if (msecs == 0)
         { ^ Task.completedTask() };

      Task promise := Task.newPromise();
      if (msecs > 0)
         TimerQueue.schedule(msecs, { promise.trySetResult() });

      ^ promise
   }

   static Task sleep(int msecs, CancellationToken token)
   {
      if (msecs < -1)
         InvalidArgumentException.raise("msecs");

      if (token == nil)
         InvalidArgumentException.raise("token");

      if:not (token.CanBeCanceled)
         { ^ Task.sleep(msecs) };

      if (token.IsCancellationRequested)
         { ^ Task.fromCanceled(token) };

      if (msecs == 0)
         { ^ Task.completedTask() };

      DelayPromise delay := DelayPromise.new(token);
      delay.schedule(msecs);

      ^ delay.Task
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

   static Task fromCanceled(CancellationToken cancellationToken)
   {
      if (cancellationToken == nil)
         InvalidArgumentException.raise("cancellationToken");

      if:not (cancellationToken.IsCancellationRequested)
         InvalidArgumentException.raise("cancellationToken");

      Task task := Task.newPromise();
      task.setCanceled(cancellationToken);

      ^ task
   }
}

internal sealed class DelayPromise
{
   Task                           _task;
   CancellationToken              _token;
   TimerRegistration?             _timer;
   CancellationTokenRegistration? _cancellationRegistration;
   bool                           _finished;

   internal constructor new(CancellationToken token)
   {
      _task := Task.newPromise();
      _token := token;
      _finished := false
   }

   internal Task Task
      = _task;

   private finish(bool canceled)
   {
      TimerRegistration? timer := nil;
      CancellationTokenRegistration? cancellationRegistration := nil;
      bool complete := false;
      lock(self) {
         if:not (_finished) {
            _finished := true;
            timer := _timer;
            _timer := nil;
            cancellationRegistration := _cancellationRegistration;
            _cancellationRegistration := nil;
            complete := true
         }
      };

      if (complete) {
         if:not:nil(timer)
            timer!.close();

         if:not:nil(cancellationRegistration)
            cancellationRegistration!.close();

         if (canceled) {
            _task.setCanceled(_token)
         }
         else {
            _task.setResult()
         }
      }
   }

   private attachTimer(TimerRegistration timer)
   {
      bool closeTimer := false;
      lock(self) {
         if (_finished) {
            closeTimer := true
         }
         else {
            _timer := timer
         }
      };

      if (closeTimer)
         timer.close()
   }

   private attachCancellation(CancellationTokenRegistration registration)
   {
      bool closeRegistration := false;
      lock(self) {
         if (_finished) {
            closeRegistration := true
         }
         else {
            _cancellationRegistration := registration
         }
      };

      if (closeRegistration)
         registration.close()
   }

   internal sealed schedule(int milliseconds)
   {
      CancellationTokenRegistration cancellationRegistration := _token.register({ finish(true) });
      attachCancellation(cancellationRegistration);

      bool addTimer := false;
      lock(self) {
         addTimer := !_finished
      };

      if (addTimer && milliseconds > 0) {
         TimerRegistration timer := TimerQueue.schedule(milliseconds, { finish(false) });
         attachTimer(timer)
      }
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

   internal constructor assignCancelable(Func<TResult> func, CancellationToken cancellationToken)
      <= assign(func)
   {
      _cancellationToken := cancellationToken
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

   static Task<TResult> run(Func<TResult> action, CancellationToken token)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      if (token == nil)
         InvalidArgumentException.raise("token");

      if:not (token.CanBeCanceled)
         { ^ class Task<TResult>.run(action) };

      if (token.IsCancellationRequested)
         { ^ class Task<TResult>.fromCanceled(token) };

      Task<TResult> task := class Task<TResult>.assignCancelable(action, token);
      task.start();

      ^ task
   }

   static Task<TResult> fromResult(TResult result)
      = new Task<TResult>(result);

   static Task<TResult> fromCanceled(CancellationToken cancellationToken)
   {
      if (cancellationToken == nil)
         InvalidArgumentException.raise("cancellationToken");

      if:not (cancellationToken.IsCancellationRequested)
         InvalidArgumentException.raise("cancellationToken");

      Task<TResult> task := class Task<TResult>.newPromise();
      task.setCanceled(cancellationToken);

      ^ task
   }
}
