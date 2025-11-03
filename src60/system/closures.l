// --- base functions ---
public interface Func
   : info("A base weak action without arguments")
{
   abstract function() : info("An action code<br/>Must be overridden");

   sealed doWhile(b) : info("Repeats the function while <b>b</b> is true")
   {
      while(b)
      {
         self()
      }
   }

   sealed doUntil(b) : info("Repeats the function until <b>b</b> is true. The code is executed at least once")
   {
      do
      {
         self()
      }
      until(b)
   }
}

public interface Func<TResult> : Func, info("A base strong-typed function without arguments")
{
   abstract TResult function() : info("A function code<br/>Must be overridden");
}

// --- Func1 ---

public interface Func1 : info("A base weak action with a single argument")
{
   abstract function(arg1) : info("A function code<br/>Must be overridden");

   run(Enumerator en)
         : info("Executes the function for every enumeration item")
   {
      while (en.next())
      {
         self(*en)
      }
   }

   retrieve(Enumerator en)
      : info("Executes the function for every enumeration item until it returns true")
   {
      object? item := nil;
      while (en.next())
      {
         item := *en;

         if (self(item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Action<T> : info("A base strong-typed action with a single argument")
{
   abstract function(T arg) : info("A function code<br/>Must be overridden");

   Func1 cast() : info("Converts to a weak function with a single argument")
      = (x => self(x));

   run(Enumerator<T> en)
         : info("Executes the function for every enumeration item")
   {
      while (en.next())
      {
         self(*en)
      }
   }
}

public interface Func<T,TResult> : Action<T>, info("A base strong-typed function with a single argument")
{
   abstract TResult function(T arg) : info("A function code<br/>Must be overridden");
}

// --- Func2 ---

public interface Func2 : info("A base weak action with two arguments")
{
   abstract function(param1, param2) : info("A function code<br/>Must be overridden");

   sealed run(object param, Enumerator en) : info("Executes the function for every enumeration item with <b>param</b> as a first argument")
   {
      while (en.next())
      {
         self(param, *en)
      }
   }

   sealed retrieve(object param, Enumerator en) : info("Executes the function for every enumeration item with <b>param</b> as a first argument until it returns true")
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

public interface Action<T1, T2> : info("A base strong-typed action with two arguments")
{
   abstract function(T1 arg1, T2 arg2) : info("A function code<br/>Must be overridden");

   Func2 cast() : info("Converts to a weak function with two arguments")
      = (x,y => self(x,y));
}

public interface Func<T1, T2,TResult> : Action<T1, T2>, info("A base strong-typed function with two arguments")
{
   abstract TResult function(T1 arg1, T2 arg2) : info("A function code<br/>Must be overridden");
}

// --- Func3 ---

public interface Func3 : info("A base weak action with three arguments")
{
   abstract function(param1, param2, param3) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, Enumerator en) 
      : info("Executes the function for every enumeration item with <b>param1</b> and <b>param2</b> as first two argument")
   {
      while (en.next())
      {
         self(param1, param2, *en)
      }
   }

   sealed retrieve(object param1, object param2, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b> and <param2> as first two argument until it returns true")
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

public interface Action<T1, T2, T3> : info("A base strong-typed action with three arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3) : info("A function code<br/>Must be overridden");

   Func3 cast() : info("Converts to a weak function with three arguments")
      = (x,y,z => self(x,y,z));
}

public interface Func<T1, T2, T3,TResult> : Action<T1, T2, T3>, info("A base strong-typed function with three arguments")
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3) : info("A function code<br/>Must be overridden");
}

// --- Func4 ---

public interface Func4 : info("A base weak action with four arguments")
{
   abstract function(param1, param2, param3, param4) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, object param3, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b> and <b>param3</b> as first three argument")
   {
      while (en.next())
      {
         self(param1, param2, param3, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b> and <b>param3</b> as first three argument until it returns true")
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

public interface Action<T1, T2, T3, T4> : info("A base strong-typed action with four arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3, T4 arg4) : info("A function code<br/>Must be overridden");

   Func4 cast() : info("Converts to a weak function with four arguments")
      = (a,b,c,d => self(a,b,c,d));
}

public interface Func<T1, T2, T3, T4,TResult> : Action<T1, T2, T3, T4>, info("A base strong-typed function with four arguments")
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4) : info("A function code<br/>Must be overridden");
}

// --- Func5 ---

public interface Func5 : info("A base weak action with five arguments")
{
   abstract function(param1, param2, param3, param4, param5) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, object param3, object param4, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b> and <b>param4</b> as first four arguments")
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b> and <b>param4</b> as first four arguments until it returns true")
   {
      while (en.next())
      {
         var item := *en;

         if (self(param1, param2, param3, param4, item))
            { ^ item }
      };

      ^ nil
   }
}

public interface Action<T1, T2, T3, T4, T5> : info("A base strong-typed action with five arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) : info("A function code<br/>Must be overridden");

   Func5 cast()
      = (a,b,c,d,e => self(a,b,c,d,e));
}

public interface Func<T1, T2, T3, T4, T5,TResult> : Action<T1, T2, T3, T4, T5>, info("A base strong-typed function with five arguments")
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5) : info("A function code<br/>Must be overridden");
}

// --- Func6 ---

public interface Func6 : info("A base weak action with six arguments")
{
   abstract function(param1, param2, param3, param4, param5, param6) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, object param3, object param4, object param5, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b> and <b>param5</b> as first five arguments")
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b> and <b>param5</b> as first five arguments until it returns true")
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

public interface Action<T1, T2, T3, T4, T5, T6> : info("A base strong-typed action with six arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6) : info("A function code<br/>Must be overridden");

   Func6 cast() : info("Converts to a weak function with six arguments")
      = (a,b,c,d,e,f => self(a,b,c,d,e,f));
}

public interface Func<T1, T2, T3, T4, T5, T6,TResult> : Action<T1, T2, T3, T4, T5, T6>, info("A base strong-typed function with six arguments")
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6) : info("A function code<br/>Must be overridden");
}

// --- Func7 ---

public interface Func7 : info("A base weak action with seven arguments")
{
   abstract function(param1, param2, param3, param4, param5, param6, param7) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b> and <b>param6</b> as first six arguments")
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b> and <b>param6</b> as first six arguments until it returns true")
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

public interface Action<T1, T2, T3, T4, T5, T6, T7> : info("A base strong-typed action with seven arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7) : info("A function code<br/>Must be overridden");

   Func7 cast() : info("Converts to a weak function with seven arguments")
      = (a,b,c,d,e,f,g => self(a,b,c,d,e,f,g));
}

public interface Func<T1, T2, T3, T4, T5, T6, T7,TResult> : Action<T1, T2, T3, T4, T5, T6, T7>, info("A base strong-typed function with seven arguments")
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7) : info("A function code<br/>Must be overridden");
}

// --- Func8 ---

public interface Func8 : info("A base weak action with eight arguments")
{
   abstract function(param1, param2, param3, param4, param5, param6, param7, param8) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, object param7, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b>, <b>param6</b> and <b>param7</b> as first seven arguments")
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, param7, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, object param7, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b>, <b>param6</b> and <b>param7</b> as first seven arguments until it returns true")
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

public interface Action<T1, T2, T3, T4, T5, T6, T7, T8> : info("A base strong-typed action with eight arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8) : info("A function code<br/>Must be overridden");

   Func8 cast() : info("Converts to a weak function with eight arguments")
      = (a,b,c,d,e,f,g,h => self(a,b,c,d,e,f,g,h));
}

public interface Func<T1, T2, T3, T4, T5, T6, T7, T8,TResult> : Action<T1, T2, T3, T4, T5, T6, T7, T8>, info("A base strong-typed function with eight arguments")
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8) : info("A function code<br/>Must be overridden");
}

// --- Func9 ---

public interface Func9 : info("A base weak action with nine arguments")
{
   abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, object param7, object param8, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b>, <b>param6</b>, <b>param7</b> and <b>param8</b> as first eight arguments")
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, param7, param8, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, object param7, object param8, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b>, <b>param6</b>, <b>param7</b> and <b>param8</b> as first eight arguments until it returns true")
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

public interface Action<T1, T2, T3, T4, T5, T6, T7, T8, T9> : info("A base strong-typed action with nine arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9) : info("A function code<br/>Must be overridden");

   Func9 cast() : info("Converts to a weak function with nine arguments")
      = (a,b,c,d,e,f,g,h,i => self(a,b,c,d,e,f,g,h,i));
}

public interface Func<T1, T2, T3, T4, T5, T6, T7, T8, T9,TResult> : Action<T1, T2, T3, T4, T5, T6, T7, T8, T9>
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9) : info("A function code<br/>Must be overridden");
}

// --- Func10 ---

public interface Func10 : info("A base weak action with ten arguments")
{
   abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, object param7, object param8, object param9, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b>, <b>param6</b>, <b>param7</b>, <b>param8</b> and <b>param9</b> as first nine arguments")
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, param7, param8, param9, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, object param7, object param8, object param9, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b>, <b>param6</b>, <b>param7</b>, <b>param8</b> and <b>param9</b> as first nine arguments until it returns true")
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

public interface Action<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> : info("A base strong-typed action with ten arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10) : info("A function code<br/>Must be overridden");

   Func10 cast() : info("Converts to a weak function with ten arguments")
      = (a,b,c,d,e,f,g,h,i,j => self(a,b,c,d,e,f,g,h,i,j));
}

public interface Func<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,TResult> : Action<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>, info("A base strong-typed function with ten arguments")
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10);
}

// --- Func11 ---

public interface Func11 : info("A base weak action with eleven arguments")
{
   abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11) : info("A function code<br/>Must be overridden");

   sealed run(object param1, object param2, object param3, object param4, object param5, object param6, object param7, 
      object param8, object param9, object param10, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b>, <b>param6</b>, <b>param7</b>, <b>param8</b>, <b>param9</b> and <b>param10</b> as first ten arguments")
   {
      while (en.next())
      {
         self(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, *en)
      }
   }

   sealed retrieve(object param1, object param2, object param3, object param4, object param5, object param6, object param7, 
      object param8, object param9, object param10, Enumerator en)
      : info("Executes the function for every enumeration item with <b>param1</b>, <b>param2</b>, <b>param3</b>, <b>param4</b>, <b>param5</b>, <b>param6</b>, <b>param7</b>, <b>param8</b>, <b>param9</b> and <b>param10</b> as first ten arguments until it returns true")
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

public interface Action<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> : info("A base strong-typed action with eleven arguments")
{
   abstract function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11) : info("A function code<br/>Must be overridden");

   Func11 cast() : info("Converts to a weak function with eleven arguments")
      = (a,b,c,d,e,f,g,h,i,j,k => self(a,b,c,d,e,f,g,h,i,j,k));
}

public interface Func<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11,TResult> : Action<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>, info("A base strong-typed function with eleven arguments")
{
   abstract TResult function(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11) : info("A function code<br/>Must be overridden");
}

// --- BaseLazyExpression ---

public abstract BaseLazyExpression
{
   abstract function() : info("An expression code<br/>Must be overridden");
    
   dispatch() : external(system'core_routines'__lazy_dispatcher);
}
