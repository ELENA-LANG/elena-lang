import system'collections;

public extension EnumConvertorOp<T> : Enumerator<T>
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