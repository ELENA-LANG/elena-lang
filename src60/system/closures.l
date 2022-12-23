// --- base functions ---
public interface Func
   : info<"A base action">
{
   abstract function();

   sealed doWhile(b)
   {
      while(b)
      {
         self()
      }
   }

   sealed doUntil(b)
   {
//      do
//      {
//         self()
//      }
//      until(b)
   }
}

public interface Func1
{
   abstract function(arg1);
}

public interface Func<T,TResult>
{
   abstract TResult function(T);
}
