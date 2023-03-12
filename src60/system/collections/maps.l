namespace collections
{
   public sealed Map<TKey, TValue> //: Enumerable<Tuple<TKey, TValue>>
   {
      int _hashMask;

      List<Tuple<TKey, TValue>> _list;
      int[][]                   _hashTable; 

      private init(int capacity, int hashLength)
      {
         _hashMask := hashLength;
            
         _hashTable := new int[][](hashLength);
         _list := new List<Tuple<TKey, TValue>>(capacity);
      }

      constructor()
      {
         self.init(32,37)
      }
      
      constructor(int capacity, int hashLength)
      {
         self.init(capacity,hashLength)
      }      
   }
}