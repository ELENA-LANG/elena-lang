import system;

public sealed class Task
{
   bool           _completed;

   Exception      _exception;
   Action<Task>   _continuation;

   internal Exception Exception
   {  
      get()
         = _exception;
   }

   continueWith(Action<Task> action)
   {
      lock(self) {
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
   }

   private complete(Exception ex)
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

   setResult()
      <= complete(nil);

   setException(Exception ex)
      <= complete(ex);

   wait()
   {
      ManualResetEvent mres := nil;
      lock (self)
      {
         ifnot(_completed)
         {
            mres := ManualResetEvent.new();
            continueWith((Task x){ mres.set() });
         }
      };
      
      mres?.wait();
      ifnotNil (_exception)
      {
         _exception.raise()
      }
   }

   static Task whenAll(params Task[] tasks)
   {
      auto t := new Task();

      int len := tasks.Length;
      int remaining := len;

      Exception e := nil;

      Action<Task> continuation := (Task t)
      {
         e := e ?? t.Exception;
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

   static Task Run(Func action)
   {
       var t = new Task();
   
       ThreadPool.QueueUserWorkItem(_ =>
       {
           try
           {
               action();
               t.SetResult();
           }
           catch (Exception e)
           {
               t.SetException(e);
           }
       });
   
       return t;
   }
}