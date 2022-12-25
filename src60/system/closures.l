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

   run(Enumerator<T> en)
         : info<"Executes the function for every enumeration item">
   {
      while (en.next())
      {
         self(*en)
      }
   }
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

   Func2 cast()
      = (x,y => self(x,y));
}

// --- Func3 ---

public interface Func3
{
   abstract function(param1, param2, param3);

   sealed run(object param1, object param2, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, *en)
      }
   }

   sealed retrieve(object param1, object param2, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3);

   Func3 cast()
      = (x,y,z => self(x,y,z));
}

// --- Func4 ---

public interface Func4
{
   abstract function(param1, param2, param3, param4);

   sealed run(object param1, object param2, object param3, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, param3, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3, T4,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4);

   Func4 cast()
      = (a,b,c,d => self(a,b,c,d));
}

// --- Func5 ---

public interface Func5
{
   abstract function(param1, param2, param3, param4, param5);

   sealed run(object param1, object param2, object param3, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, param3, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3, T4, T5,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5);

   Func5 cast()
      = (a,b,c,d,e => self(a,b,c,d,e));
}

// --- Func6 ---

public interface Func6
{
   abstract function(param1, param2, param3, param4, param5, param6);

   sealed run(object param1, object param2, object param3, object param4, object param5, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, param4, param5, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3, T4, T5, T6,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6);

   Func6 cast()
      = (a,b,c,d,e,f => self(a,b,c,d,e,f));
}

// --- Func7 ---

public interface Func7
{
   abstract function(param1, param2, param3, param4, param5, param6, param7);

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, param4, param5, param6, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3, T4, T5, T6, T7,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7);

   Func7 cast()
      = (a,b,c,d,e,f,g => self(a,b,c,d,e,f,g));
}

// --- Func8 ---

public interface Func8
{
   abstract function(param1, param2, param3, param4, param5, param6, param7, param8);

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, object param7, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, param7, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, object param7, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, param4, param5, param6, param7, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3, T4, T5, T6, T7, T8,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8);

   Func8 cast()
      = (a,b,c,d,e,f,g,h => self(a,b,c,d,e,f,g,h));
}

// --- Func9 ---

public interface Func9
{
   abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9);

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, object param7, object param8, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, param7, param8, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, object param7, object param8, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, param4, param5, param6, param7, param8, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3, T4, T5, T6, T7, T8, T9,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9);

   Func9 cast()
      = (a,b,c,d,e,f,g,h,i => self(a,b,c,d,e,f,g,h,i));
}

// --- Func10 ---

public interface Func10
{
   abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, object param7, object param8, object param9, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, param7, param8, param9, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, object param7, object param8, object param9, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, param4, param5, param6, param7, param8, param9, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10);

   Func10 cast()
      = (a,b,c,d,e,f,g,h,i,j => self(a,b,c,d,e,f,g,h,i,j));
}

// --- Func11 ---

public interface Func11
{
   abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, object param7, 
      object param8, object param9, object param10, Enumerator en)
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, object param7, 
      object param8, object param9, object param10, Enumerator en)
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Func<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,TResult>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11);

   Func11 cast()
      = (a,b,c,d,e,f,g,h,i,j,k => self(a,b,c,d,e,f,g,h,i,j,k));
}