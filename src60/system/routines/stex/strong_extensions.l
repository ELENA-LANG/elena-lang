public extension strongEnumerableOp<T>
{
   Enumerator<T> filterBy(Func<T,bool> f)
      = new StrongFilter<T>(f, cast Enumerator<T>(self.enumerator()));
}