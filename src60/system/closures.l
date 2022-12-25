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
      do
      {
         self()
      }
      until(b)
   }
}

// --- Func1 ---

public interface Func1
{
   abstract function(arg1);

   run(Enumerator en)
         : info<"Executes the function for every enumeration item">
   {
      while (en.next())
      {
         self(*en)
      }
   }

   retrieve(Enumerator en)
   {
      object item := nil;
      while (en.next())
      {
         item := *en;

         if (self(item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T,TResult>
{
   abstract TResult function(T arg);

   Func1 cast()
      = (x => self(x));

//   run(Enumerator<T> en)
//         : info<"Executes the function for every enumeration item">
//   {
//      while (en.next())
//      {
//         self(*en)
//      }
//   }
}

// --- Func2 ---

public interface Func2
{
   abstract function(param1, param2);

   sealed run(object param, Enumerator en)
   {
      while (en.next())
      {
         self(param, *en)
      }
   }

   sealed retrieve(object param, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2);

//   Func2 cast()
//      = (x,y => self(x,y));
}
