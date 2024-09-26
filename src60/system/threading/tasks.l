import system;

public closed class Task
{
   bool           _completed;

   Exception      _exception;
   Action<Task>   _continuation;

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

   get Result()
   {
      wait();

      ^ nil;
   }      

   internal Exception Exception
   {  
      get()
         = _exception;
   }

   private continueWithUnsafe(Action<Task> action)
   {
      if (_completed) {
         ThreadPool.queueAction({ action(self) });
      }
      else ifnotNil(_continuation) {
         InvalidOperationException.raise("Only a single continuation is supported currently")
      }
      else {
         _continuation := action;            
      }
   }

   sealed continueWith(Action<Task> action)
   {
      lock(self) {
         continueWithUnsafe(action);
      }
   }

   private complete(Exception? ex)
   {
      lock(self) {
         if (_completed) {
            InvalidOperationException.raise("The task is already completed")
         };

         _exception := ex;
         _completed := true;

         ifnotNil(_continuation) {
            _continuation(self);
         }
      }
   }

   internal sealed setResult()
      <= complete(nil);

   internal sealed setException(Exception ex)
      <= complete(ex);

   sealed wait()
   {
      ManualResetEvent mres := nil;
      lock (self)
      {
         ifnot(_completed)
         {
            mres := ManualResetEvent.new();
            continueWithUnsafe((Task x){ mres.set() });
         }
      };
      
      mres?.wait();
      ifnotNil (_exception)
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
   {
      auto t := new Task();

      int len := tasks.Length;
      int remaining := len;

      Exception e := nil;

      Action<Task> continuation := (Task completed)
      {
         e := e ?? completed.Exception;
         lock(t) {
            remaining := remaining - 1;
         };
         if (remaining == 0) {
            ifnotNil (e) {
               t.setException(e)
            }
            else t.setResult();
         };
      };

      for (int i := 0; i < len; i++) {
         tasks[i].continueWith(continuation);
      };

      ^ t;      
   }

   static Task whenAll(Task[] tasks)
   {
      auto t := new Task();

      int len := tasks.Length;
      int remaining := len;

      Exception e := nil;

      Action<Task> continuation := (Task completed)
      {
         e := e ?? completed.Exception;
         lock(t) {
            remaining := remaining - 1;
         };
         if (remaining == 0) {
            ifnotNil (e) {
               t.setException(e)
            }
            else t.setResult();
         };
      };

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
               t.setException(e);
           }
       });
   
       ^ t;
   }

   static Task sleep(int msecs)
      = run({ Thread.sleep(msecs); });
}

// --- Task<TResult> ---

public sealed class Task<TResult> : Task
{
   TResult _result;

   constructor assign(Func<TResult> func)
   {
      _action := 
      {
         TResult retVal := func();

         setResultValue(retVal);
      };
   }

   private setResultValue(TResult r)
   {
      lock(self) {
         _result := r
      }
   }

   get TResult Result()
   {
      wait();

      ^ _result;
   }
}