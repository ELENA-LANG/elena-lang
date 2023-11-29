namespace collections
{
   public sealed Map<TKey, TValue> : Enumerable<Tuple<TKey, TValue>>
   {
      int _hashMask;

      List<VarTuple<TKey, TValue>> _list;
      int[][]                   _hashTable; 

      private init(int capacity, int hashLength)
      {
         _hashMask := hashLength;
            
         _hashTable := new int[][](hashLength);
         _list := new List<VarTuple<TKey, TValue>>(capacity);
      }

      constructor()
      {
         self.init(32,37)
      }
      
      constructor(int capacity, int hashLength)
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
         for (int i := 1, i < len, i += 1)
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

      setAt(TKey key, TValue object)
      {
         int code := key.Hashcode.Absolute;
    
         code := code.mod(_hashMask);
            
         auto item := self.retrieve(code, key);
            
         item.Item2 := object
      }
      
      TValue at(TKey key)
      {
         int code := key.Hashcode.Absolute;
    
         code := code.mod(_hashMask);
            
         auto item := self.retrieve(code, key);
            
         ^ item.Item2
      }
          
      /// Returns a dictionary enumerator
      Enumerator<Tuple<TKey, TValue>> enumerator() => _list;
        
      Array<VarTuple<TKey, TValue>> Values = _list.Value;
    
      get TValue[] MapValues()
      {
         auto list := new List<TValue>();
         auto it := _list.enumerator();
         while (it.next())
         {
            list.append((*it).Item2)
         };
            
         ^ list.Value
      }
    
    
      /// Clones a dictionary
      clone() 
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
        
      clear()
      {
         _hashTable := new int[][](_hashTable.Length);
         _list.clear()
      }
        
      bool containsKey(TKey key)
      {
         int code := key.Hashcode.Absolute;
    
         code := code.mod(_hashMask);
            
         auto backet := _hashTable[code];
         if (nil == backet)
         {
            ^ false
         };
            
         int len := backet[0];
         for (int i := 1, i < len, i += 1)
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