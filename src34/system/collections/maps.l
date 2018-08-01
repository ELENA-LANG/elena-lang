/// Map template
public sealed Map<key_type, value_type> :: Enumerable<Tuple<key_type, value_type>>
{
    int theHashMask.
    
    List<Tuple<key_type, value_type>> theList.
    Array<IntArray>                   theHashTable. 
    
//    value_type     theDefault.

    private stacksafe init(int capacity, int hashLength)
    [
        int size := 32.
        while (size < hashLength)
        [
            size := size << 1.
        ].

        size := size - 1.
        theHashMask := IntNumber new(size).
        
        theHashTable := V<IntArray>(hashLength).
        theList := List<Tuple<key_type, value_type>>(capacity).
        
//        theDefault := aDefault.
    ]

    constructor
    [
        self init(32,32).
    ]

    stacksafe constructor(int capacity, int hashLength)
    [
        self init(capacity,hashLength).
    ]
    
    private Tuple<key_type, value_type> retrieve(int index, key_type key)
    [
        T<IntArray> backet := theHashTable[index].        
        if (nil == backet)
        [
            backet := V<int>(5).
            backet[0] := 1.
            
            theHashTable[index] := backet.
        ].
        
        int len := 0.
        backet read(0, &len).
        int i := 1.
        while (i < len)
        [
            int j := 0.
            backet read(i, &j).
            
            Tuple<key_type, value_type> item := theList[j].
            
            if(key equal(item item1))
                [ ^ item ].
            
            i := i + 1
        ].

        Tuple<key_type, value_type> new_item := Tuple<key_type, value_type>(key).

        i := theList length.
        theList append(new_item).
        
        // Extend a backet
        int capacity := backet length.
        if (capacity == len)
        [
            auto new_backet := IntArray new(capacity + 4).
            new_backet write(0, capacity, backet).

            backet := new_backet.
            theHashTable[index] := new_backet.
        ].
            
        backet write(len, i).
            
        len += 1.            
        backet write(0, len).

        ^ new_item.                
    ]
    
    setAt(key_type aKey, value_type anObject)
    [
        int code := 0.
        aKey readHashCodeTo(&code).

        code := code && theHashMask.
        
        Tuple<key_type, value_type> item := self retrieve(code, aKey).
        
        item item2 := anObject.
    ]
        
    value_type getAt(key_type aKey)
    [
        int code := 0.
        aKey readHashCodeTo(&code).

        code := code && theHashMask.
        
        Tuple<key_type, value_type> item := self retrieve(code, aKey).
        
        ^ item item2.
    ]

    /// Returns a dictionary enumerator
    Enumerator<Tuple<key_type, value_type>> enumerator => theList.
    
//    array_list
//        = theList clone.
//
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
    
    clear
    [
        theHashTable := V<IntArray>(theHashTable length).
        theList clear.
    ]
}
