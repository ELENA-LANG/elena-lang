public StrongFilter<T> : Enumerator<T>
{
   Func<T,bool>  filter;
   Enumerator<T> enumerator;
    
   constructor(Func<T,bool> filter, Enumerator<T> enumerator)
   {
      this filter := filter;
      this enumerator := enumerator;
   }
    
   T get() => enumerator;
    
   bool next()
   {
      while (enumerator.next())
      {
         if (filter(enumerator.get()))
            { ^ true }
      };
        
      ^ false
   }    
    
   reset() => enumerator;
    
   enumerable() => enumerator;
    
   Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}
