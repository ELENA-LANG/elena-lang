import system;

public extension strongEnumerableOp<T>
{
   Enumerator<T> filterBy(Func<T,bool> f)
      = new StrongFilter<T>(f, cast Enumerator<T>(self.enumerator()));
        
    Enumerator<T> selectBy(Func<T,T> f)
        = new StrongSelector<T,T>(f, cast Enumerator<T>(self.enumerator()));
        
    Enumerator<T> selectBy(Func<T,object> f)
        = new StrongSelector<T,object>(f, cast Enumerator<T>(self.enumerator()));
}