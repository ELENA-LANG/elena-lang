public abstract class AsyncStateEnumerator : Enumerator<Task>
{
   private __context;  // context must be the first one
   private Task __current; 

   get Task Value()
      = __current;

   reset()
   {
      NotSupportedException.raise()
   }

   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };

   enumerable() = self;

   static Task proceed(Enumerator<Task> e)
   {
      Task t := Task.newPromise();

      Action<Task> proceeding := (Task tsk)
      {
         try
         {
             if (e.next()) {
                (*e).continueWith(this self);

                ^ this self;
             };

             t.setResult(); 
         }
         catch (Exception e)
         {
            t.setException(e);
         };
      };

      proceeding(t);

      ^ t;  
   }
}

public abstract class AsyncStateEnumerator<TResult> : Enumerator<Task>
{
   private __context;  // context must be the first one
   private Task __current; 

   get Task Value()
      = __current;

   reset()
   {
      NotSupportedException.raise()
   }

   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };

   enumerable() = self;

   static Task<TResult> proceed(Enumerator<Task> e)
   {
      Task<TResult> t := class Task<TResult>.newPromise();

      Action<Task> proceeding := (Task tsk)
      {
         try
         {
             if (e.next()) {
                (*e).continueWith(this self);

                ^ this self;
             }
             else t.setResultValue((*e).getResultUnsafe());

             t.setResult(); 
         }
         catch (Exception e)
         {
            t.setException(e);
         };
      };

      proceeding(t);

      ^ t;  
   }
}
