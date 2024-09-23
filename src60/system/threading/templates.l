import system;

public abstract class AsyncStateEnumerator: Enumerator<Task>
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
      Task t := new Task();

      Action<Task> proceeding := (Task tsk)
      {
         try
         {
             if (e.next()) {
                (*e).continueWith(this self);

                ^ this self;
             }
         }
         catch (Exception e)
         {
            t.setException(e);
         };

         t.setResult(); 
      };

      proceeding(t);

      ^ t;  
   }
}
