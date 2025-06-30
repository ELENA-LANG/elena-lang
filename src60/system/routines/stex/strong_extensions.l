import system;

public extension strongEnumerableOp<T>
{
   forEach(Action<T> f)
   {
      Enumerator<T> e := self.enumerator();
        
      while (e.next())
      {
         f(*e)
      }
   }

   Enumerator<T> filterBy(Func<T,bool> f)
      = new StrongFilter<T>(f, cast Enumerator<T>(self.enumerator()));
        
   Enumerator<T> selectBy(Func<T,T> f)
      = new StrongSelector<T,T>(f, cast Enumerator<T>(self.enumerator()));
}

public extension strongEnumerableOp2<T>
{
   T summarize(T defValue)
   {
      auto p := new StrongSumming<T>(defValue);
        
      ^ p.run(cast Enumerator<T>(self.enumerator()))
   }
}

public extension strongEnumerableOp<T1,T2>
{
    Enumerator<T2> selectBy(Func<T1,T2> f)
        = new StrongSelector<T1,T2>(f, cast Enumerator<T1>(self.enumerator()));    
}

// --- Summing ---

public StrongSumming<T> : Action<T>
{
   Reference<T> result;
    
   constructor(T defValue)
   {
      result := defValue
   }

   run(Enumerator<T> e)
   {
      super.run(e);
        
      ^ result.Value
   }

   function(T item)
   { 
      T val := result.Value;
      val := val + item;
        
      result.Value := val
   }
}

