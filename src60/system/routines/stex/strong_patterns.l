import system;

public StrongFilter<T> : Enumerator<T>
{
   Func<T,bool>  filter;
   Enumerator<T> enumerator;
    
   constructor(Func<T,bool> filter, Enumerator<T> enumerator)
   {
      this filter := filter;
      this enumerator := enumerator;
   }
    
   //get T Value() => enumerator;
   T Value
      = *enumerator;
    
   bool next()
   {
      while (enumerator.next())
      {
         if (filter(*enumerator))
            { ^ true }
      };
        
      ^ false
   }    
    
   reset() => enumerator;
    
   enumerable() => enumerator;
    
   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}

// --- StrongSelector ---

public StrongSelector<T1, T2> : Enumerator<T2>
{
   Func<T1,T2> selector;
   Enumerator<T1> enumerator;

   constructor(Func<T1,T2> selector, Enumerator<T1> enumerator)
   {
      this selector := selector;
      this enumerator := enumerator;
   }
    
   get T2 Value() = selector(*enumerator);
    
   bool next() = enumerator.next();
    
   reset() => enumerator;
    
   enumerable() => enumerator;
    
   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}
