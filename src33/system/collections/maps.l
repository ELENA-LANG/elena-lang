/// Map template
sealed template<BaseEnumerable> map :: key_type, value_type
{
    int theHashMask.
    
    list<tuple<key_type, value_type>> theList.
    array<intarray>                   theHashTable. 
    
//    value_type     theDefault.

    sealed $init(int capacity, int hashLength)
    [
        int size := 32.
        while (size < hashLength)
        [
            size := size << 1.
        ].

        size := size - 1.
        theHashMask := IntNumber new int:size.
        
        theHashTable := intarray<>(hashLength).
        theList := list<tuple<key_type, value_type>>(capacity).
        
//        theDefault := aDefault.
    ]

    implicit
    [
        $self $init(32,32).
    ]

    implicit int:capacity int:hashLength// object:aDefault
    [
        $self $init(capacity,hashLength).
    ]
    
    sealed tuple<key_type, value_type> $retrieve(int index, key_type key)
    [
        intarray backet := theHashTable[index].        
        if ($nil == backet)
        [
            backet := int<>(5).
            backet[0] := 1.
            
            theHashTable[index] := backet.
        ].
        
        int len := 0.
        backet read int:0 vint:len.
        int i := 1.
        while (i < len)
        [
            int j := 0.
            backet read int:i vint:j.
            
            tuple<key_type, value_type> item := theList[j].
            
            if(key equal(item item1))
                [ ^ item ].
            
            i := i + 1
        ].

        tuple<key_type, value_type> new_item := tuple<key_type, value_type>(key).

        i := theList length.
        theList append(new_item).
        
        // Extend a backet
        int capacity := backet length.
        if (capacity == len)
        [
            intarray new_backet := IntArray new int:(capacity + 4).
            new_backet write int:0 int:capacity intarray:backet.

            backet := new_backet.
            theHashTable[index] := new_backet.
        ].
            
        backet write int:len int:i.
            
        len += 1.            
        backet write int:0 int:len.

        ^ new_item.                
    ]
    
    setAt key_type:aKey value_type:anObject
    [
        int code := 0.
        aKey readHashCodeTo vint:code.

        code := code && theHashMask.
        
        tuple<key_type, value_type> item := $self $retrieve(code, aKey).
        
        item item2 := anObject.
    ]
        
    getAt key_type:aKey
    [
        int code := 0.
        aKey readHashCodeTo vint:code.

        code := code && theHashMask.
        
        tuple<key_type, value_type> item := $self $retrieve(code, aKey).
        
        ^ item item2.
    ]

    /// Returns a dictionary enumerator
    enumerator => theList.
    
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
        theHashTable := intarray<>(theHashTable length).
        theList clear.
    ]
}
