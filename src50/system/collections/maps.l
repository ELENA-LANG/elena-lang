namespace collections
{
    /// Map template
    public sealed Map<TKey, TValue> : Enumerable<Tuple<TKey, TValue>>
    {
        int theHashMask;
        
        List<Tuple<TKey, TValue>> theList;
        int[][]                   theHashTable; 
        
        private init(int capacity, int hashLength)
        {
            theHashMask := hashLength;
            
            theHashTable := new int[][](hashLength);
            theList := new List<Tuple<TKey, TValue>>(capacity);
        }

        constructor()
        {
            self.init(32,37)
        }
    
        constructor(int capacity, int hashLength)
        {
            self.init(capacity,hashLength)
        }
        
        private Tuple<TKey, TValue> retrieve(int index, TKey key)
        {
            auto backet := theHashTable[index];
            if (nil == backet)
            {
                backet := new int[](5);
                backet[0] := 1;
                
                theHashTable[index] := backet
            };
            
            int len := backet[0];
            for (int i := 1, i < len, i += 1)
            {
                int j := backet[i];
                
                auto item := theList[j];
                
                if(key.equal(item.Item1))
                    { ^ item }
            };
    
            auto new_item := new Tuple<TKey, TValue>(key);
    
            int i := theList.Length;
            theList.append(new_item);
            
            // Extend a backet
            int capacity := backet.Length;
            if (capacity == len)
            {
                auto new_backet := new int[](capacity + 4);
                new_backet.write(0, capacity, backet);
    
                backet := new_backet;
                theHashTable[index] := new_backet
            };
                
            backet[len] := i;
                
            len += 1;    
            backet[0] := len;
    
            ^ new_item                
        }
        
        setAt(TKey key, TValue object)
        {
            int code := key.Hashcode.Absolute;
    
            code := code.mod(theHashMask);
            
            auto item := self.retrieve(code, key);
            
            item.Item2 := object
        }
            
        TValue at(TKey key)
        {
            int code := key.Hashcode.Absolute;
    
            code := code.mod(theHashMask);
            
            auto item := self.retrieve(code, key);
            
            ^ item.Item2
        }
    
        /// Returns a dictionary enumerator
        Enumerator<Tuple<TKey, TValue>> enumerator() => theList;
        
        Array<Tuple<TKey, TValue>> Values = theList.Value;
    
        get TValue[] MapValues()
        {
            auto list := new List<TValue>();
            auto it := theList.enumerator();
            while (it.next())
            {
                list.append(*it.Item2)
            };
            
            ^ list.Value
        }
    
    
        /// Clones a dictionary
        clone() 
        {
            auto list := new Map<TKey, TValue>();
    
            auto it := theList.enumerator();
            while (it.next())
            {
                var item := *it;
                
                list[item.Item1] := item.Item2;
            };
            
            ^ list
        }
        
        clear()
        {
            theHashTable := new int[][](theHashTable.Length);
            theList.clear()
        }
        
        bool containsKey(TKey key)
        {
            int code := key.Hashcode.Absolute;
    
            code := code.mod(theHashMask);
            
            auto backet := theHashTable[code];
            if (nil == backet)
            {
                ^ false
            };
            
            int len := backet[0];
            for (int i := 1, i < len, i += 1)
            {
                int j := backet[i];
                
                auto item := theList[j];
                
                if(key.equal(item.Item1))
                    { ^ true };
            };
            
            ^ false
        }    
    }
}