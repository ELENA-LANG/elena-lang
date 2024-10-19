import system'collections;

public extension EnumerableConvertorOp<T> : Enumerator<T>
{
   T[] toArray()
   {
      auto list := new List<T>();

      foreach(T item; in self) {
         list.append(item);
      };

      ^ list.Value
   }
}

public extension EnumeratorConvertorOp<T> : Enumerable<T>
{
   T[] toArray()
   {
      auto list := new List<T>();

      foreach(T item; in self) {
         list.append(item);
      };

      ^ list.Value
   }
}