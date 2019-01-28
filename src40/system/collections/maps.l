/// Map template
public sealed Map<TKey, TValue> : Enumerable<Tuple<TKey, TValue>>
{
    int theHashMask;
    
    List<Tuple<TKey, TValue>> theList;
    Array<int[]>              theHashTable; 
    
//    value_type     theDefault.

    private init(int capacity, int hashLength)
    {
        int size := 32;
        while (size < hashLength)
        {
            size := size $shl 1;
        };

        size := size - 1;
        theHashMask := new IntNumber(size);
        
        theHashTable := new Array<int[]>(hashLength);
        theList := new List<Tuple<TKey, TValue>>(capacity);
        
//        theDefault := aDefault.
    }

    constructor()
    {
        self.init(32,32)
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

        i := theList.Length;
        theList.append(new_item);
        
        // Extend a backet
        int capacity := backet.Length;
        if (capacity == len)
        {
            auto new_backet := new Array<int[]>(capacity + 4);
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
        int code := key.HashCode;

        code := code && theHashMask;
        
        auto item := self.retrieve(code, key);
        
        item.Item2 := object
    }
        
    TValue at(TKey key)
    {
        int code := key.HashCode;

        code := code && theHashMask;
        
        auto item := self.retrieve(code, key);
        
        ^ item.Item2
    }

    /// Returns a dictionary enumerator
    Enumerator<Tuple<TKey, TValue>> enumerator() => theList;
    
    Array<Tuple<TKey, TValue>> Values = theList.Value;

//    /// Clones a dictionary
//    clone 
//    [
//        var aList := Dictionary new.
//
//        enumerator it := theList enumerator.
//        while (it next)
//        [
//            var anItem := it get.
//            
//            aList[anItem key] := anItem value.
//        ].
//        
//        ^ aList.
//    ]
    
    clear()
    {
        theHashTable := new Array<int[]>(theHashTable.Length);
        theList.clear()
    }
    
    bool containsKey(TKey key)
    {
        int index := key.HashCode;

        index := index && theHashMask;
        
        auto backet := theHashTable[index];
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
