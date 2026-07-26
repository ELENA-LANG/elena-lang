namespace collections
{
   public sealed Map<TKey, TValue> : Enumerable<VarTuple<TKey, TValue>>, info("A strong-typed map template")
   {
      int _hashMask;

      List<VarTuple<TKey, TValue>> _list;
      int[][]                      _hashTable; 
      Func<object, int>            _hashFunction;

      private init(int capacity, int hashLength)
      {
         _hashMask := hashLength;
            
         _hashTable := new int[][](hashLength);
         _list := new List<VarTuple<TKey, TValue>>(capacity);
         _hashFunction := (key => key.Hashcode.Absolute);
      }

      constructor new(Func<object, int> hashFunction) : info("Creates a map with a default size (37) and a capacity (32); the custom hash function is provided")
      {
         _hashFunction := hashFunction;
      }

      constructor() : info("Creates a map with a default size (37) and a capacity (32)")
      {
         self.init(32,37)
      }
      
      constructor(int capacity, int hashLength) : info("Creates a map with the provided capacity and the length")
      {
         self.init(capacity,hashLength)
      }      

      private VarTuple<TKey, TValue> retrieve(int index, TKey key)
      {
         auto backet := _hashTable[index];
         if (nil == backet)
         {
            backet := new int[](5);
            backet[0] := 1;
                
            _hashTable[index] := backet
         };
            
         int len := backet[0];
         for (int i := 1; i < len; i += 1)
         {
            int j := backet[i];
                
            auto item := _list[j];
                
            if(key.equal(item.Item1))
               { ^ item }
         };
    
         TValue defVal := default;
         auto new_item := new VarTuple<TKey, TValue>(key, defVal);
    
         int i := _list.Length;
         _list.append(new_item);
            
         // Extend a backet
         int capacity := backet.Length;
         if (capacity == len)
         {
            auto new_backet := new int[](capacity + 4);
            Array<int>.copy(new_backet, backet, 0, capacity);
            //new_backet.write(0, capacity, backet);
    
            backet := new_backet;
            _hashTable[index] := new_backet
         };
                
         backet[len] := i;
                
         len += 1;    
         backet[0] := len;
         
         ^ new_item                
      }

      setAt(TKey key, TValue object) : info("Adds or updates a dictionary element with <b>key</b>")
      {
         int code := key.Hashcode.Absolute;
    
         code := code.mod(_hashMask);
            
         auto item := self.retrieve(code, key);
            
         item.Item2 := object
      }
      
      TValue at(TKey key) : info("Returns a dictionary element with <b>key</b>")
      {
         int code := key.Hashcode.Absolute;
    
         code := code.mod(_hashMask);
            
         auto item := self.retrieve(code, key);
            
         ^ item.Item2
      }
          
      /// Returns a dictionary enumerator
      Enumerator<VarTuple<TKey, TValue>> enumerator() : info("Returns an enumerator of a key-value pairs") => _list;
        
      Array<VarTuple<TKey, TValue>> Values : info("Returns an array of a key-value pairs")
         = _list.Value;
    
      get TValue[] MapValues() : info("Returns an array of a map values")
      {
         auto list := new List<TValue>();
         auto it := _list.enumerator();
         while (it.next())
         {
            list.append((*it).Item2)
         };
            
         ^ list.Value
      }
    
    
      clone() : info("Makes a shallow copy a map")
      {
         auto list := new Map<TKey, TValue>();
    
         auto it := _list.enumerator();
         while (it.next())
         {
            var item := *it;
                
            list[item.Item1] := item.Item2;
         };
            
         ^ list
      }
        
      clear() : info("Removes all key-value paires")
      {
         _hashTable := new int[][](_hashTable.Length);
         _list.clear()
      }
        
      bool containsKey(TKey key) : info("Returns true if the map contains the provided key")
      {
         int code := key.Hashcode.Absolute;
    
         code := code.mod(_hashMask);
            
         auto backet := _hashTable[code];
         if (nil == backet)
         {
            ^ false
         };
            
         int len := backet[0];
         for (int i := 1; i < len; i += 1)
         {
            int j := backet[i];
                
            auto item := _list[j];
                
            if(key.equal(item.Item1))
               { ^ true };
         };
            
         ^ false
      }    
   }
}