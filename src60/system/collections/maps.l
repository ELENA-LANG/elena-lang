namespace collections
{
   public sealed Map<TKey, TValue> //: Enumerable<Tuple<TKey, TValue>>
   {
      int _hashMask;

      List<Tuple<TKey, TValue>> _list;
//      int[][]                   _hashTable; 
   }
}