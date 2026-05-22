import system'collections;

public class Task
{
   bool           _completed;

   Exception?     _exception;
   ArrayList      _continuations := new ArrayList();

   Func           _action; 

   protected constructor()
   {
      _completed := false;
   }

   internal constructor newPromise()
   {      
   }

   constructor assign(Func action)
   {
      _action := action;
   }

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
         = _exception;
   }

   private queueContinuation(Action<Task> action)
   {
      ThreadPool.queueAction({ action(self); });
   }

   private continueWithUnsafe(Action<Task> action)
   {
      if (_completed) {
         queueContinuation(action);
      }
      else {
         _continuations.append(action);
      }
   }

   sealed continueWith(Action<Task> action)
   {
      bool executeNow := false;
      lock(self) {
         if (_completed) {
            executeNow := true;
         }
         else {
            _continuations.append(action);
         }
      };

      if (executeNow) {
         queueContinuation(action);
      }
   }

   private complete(Exception? ex)
   {
      ArrayList contCopy := new ArrayList();
      lock(self) {
         if (_completed) {
            Console.writeLine("The task is already completed");

            InvalidOperationException.raise("The task is already completed")
         };

         _exception := ex;
         _completed := true;

         contCopy := _continuations.clone();
         _continuations.clear();
      };

      int len := contCopy.Length;
      for (int i := 0; i < len; i++) {
         queueContinuation(cast Action<Task>(contCopy.at(i)));
      }
   }

   internal sealed setResult()
      <= complete(nil);

   internal sealed setException(Exception ex)
      <= complete(ex);

   sealed wait()
   {
      ManualResetEvent? mres := nil;
      lock (self)
      {
         if:not(_completed)
         {
            mres := ManualResetEvent.new();
            continueWithUnsafe((Task x){ mres!.set() });
         }
      };
      
      mres?.wait();
      if:not:nil (_exception)
      {
         _exception.raise()
      }
   }

   sealed start()
   {
      ThreadPool.queueAction(
      {
         try
         {
            _action();
            setResult();
         }
         catch (Exception e)
         {
            setException(e);
         }
      });
   }

   static Task whenAllArgs(params Task[] tasks)
      = whenAll(tasks);

   static Task whenAll(Task[] tasks)
   {
      auto t := new Task();

      int len := tasks.Length;
      if (len == 0) {
         t.setResult();

         ^ t
      };

      auto helper := new WhenAllHelper(t, len);
      Action<Task> continuation := new WhenAllContinuation(helper);

      for (int i := 0; i < len; i++) {
         tasks[i].continueWith(continuation);
      };

      ^ t;      
   }

   static waitAll(Task[] tasks)
   {
      auto t := whenAll(tasks);

      t.wait()
   }

   static waitAllArgs(params Task[] tasks)
   {
      auto t := whenAllArgs(params tasks);

      t.wait()
   }

   static Task run(Func action)
   {
       auto t := new Task();
   
       ThreadPool.queueAction(
       {
           try
           {
               action();
               t.setResult();
           }
           catch (Exception e)
           {
               Console.writeLine(e);
              
               t.setException(e);
           }
       });
   
       ^ t;
   }

   static Task sleep(int msecs)
      = run({ Thread.sleep(msecs); });
}

internal sealed class WhenAllHelper
{
   Task       _task;
   int        _remaining;
   Exception? _exception;

   constructor(Task task, int remaining)
   {
      _task := task;
      _remaining := remaining;
   }

   proceed(Task completed)
   {
      bool completeNow := false;
      Exception? exception := nil;

      lock(_task) {
         _exception := _exception ?? completed.Exception;
         _remaining := _remaining - 1;
         if (_remaining == 0) {
            completeNow := true;
            exception := _exception;
         }
      };

      if (completeNow) {
         if:not:nil (exception) {
            _task.setException(exception!)
         }
         else {
            _task.setResult();
         }
      };
   }
}

internal sealed class WhenAllContinuation : Action<Task>
{
   WhenAllHelper _helper;

   constructor(WhenAllHelper helper)
   {
      _helper := helper;
   }

   function(Task completed)
   {
      _helper.proceed(completed);
   }
}

// --- Task<TResult> ---

public sealed class Task<TResult> : Task
{
   TResult _result;

   protected constructor()
   {
      _completed := false;
   }

   constructor(TResult retVal) 
   {
      _result := retVal;
      _completed := true;
   }

   internal constructor newPromise()
   {
   }

   constructor assign(Func<TResult> func)
   {
      _action := 
      {
         TResult retVal := func();

         setResultValue(retVal);
      };
   }

   indexed internal setResultValue(TResult r)
   {
      lock(self) {
         _result := r
      }
   }

   indexed internal TResult getResultUnsafe()
      = _result;

   indexed get TResult Result()
   {
      wait();

      ^ _result;
   }

   TResult cast()
      = self.Result;

   static Task<TResult> run(Func<TResult> action)
   {
      auto t := class Task<TResult>.assign(action);

      t.start();

      ^ t;
   }
}
