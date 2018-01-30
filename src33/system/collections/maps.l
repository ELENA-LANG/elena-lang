///// Map template
//template<BaseEnumerable> map :: key_type, value_type
//{
//    int        theHashMask.
//    array<array<tuple<key_type, value_type> > > theHashTable.
//    
//    
////    array_list theList.
//    
////    object     theDefault.
//
////    /// Creates a dictionary
////    constructor new
////        <= new int:32 int:32 object:$nil.
////
////    constructor new default:aDefault
////        <= new int:32 int:32 object:aDefault.
////
////    constructor new:aCapacity default:aDefault
////        <= new int:(aCapacity int) int:32 object:aDefault.
////
////    /// Creates a dictionary
////    constructor new int:aCapacity int:aHashLength object:aDefault
////    [
////        int size := 32.
////        while (size < aHashLength)
////        [
////            size := size << 1.
////        ].
////        
////        size := size - 1.
////        theHashMask := IntNumber new int:size.
////        
////        theHashTable := Array new int:aHashLength.
////        theList := ArrayList new int:aCapacity.
////        
////        theDefault := aDefault.
////    ]
//    
//    setAt key_type:aKey value_type:anObject
//    [
//        int code := 0.
//        aKey readHashCodeTo vint:code.
//
//        code := code && theHashMask.
//        
//                        
////        $self $getItem int:aHashCode key_type:aKey; set value:anObject
//    ]
//        
////    stacksafe $getItem int:aCode key_type:aKey
////    [
////        int aHashCode := aCode && theHashMask.
////        
////        var aHashItem := theHashTable getAt int:aHashCode.
////        if ($nil == aHashItem)
////        [
////            aHashItem := IntArray new int:5.
////            
////            aHashItem write int:0 int:1.
////            
////            theHashTable setAt int:aHashCode object:aHashItem
////        ].
////        
////        intarray aBacket := aHashItem intarray.        
////        int      aLength := 0.
////        aBacket read int:0 vint:aLength.
////        
////        int i := 1.
////        while (i < aLength)
////        [
////            int anItemIndex := 0.
////            aBacket read int:i vint:anItemIndex.
////            
////            var anItem := theList getAt int:anItemIndex.
////            
////            if(aKey equal(anItem key))
////                [ ^ anItem ].
////            
////            i := i + 1
////        ].
////        
////        var aNewItem := KeyValue new key:aKey value:theDefault.
////        
////        theList readLengthTo vint:i.
////        theList append:aNewItem.
////        
////        // Extend a backet
////        int aCapacity := 0.
////        aBacket readLengthTo vint:aCapacity.
////        if (aCapacity == aLength)
////            [
////                intarray aNewBacket := IntArray new int:(aCapacity + 5).
////                aNewBacket write int:0 int:aCapacity intarray:aBacket.
////
////                aBacket := aNewBacket.
////                theHashTable setAt int:aHashCode object:aNewBacket.
////            ].
////            
////        aBacket write int:aLength int:i.
////            
////        aLength := aLength + 1.
////        aBacket write int:0 int:aLength.
////        
////        ^ aNewItem.
////    ]    
////        
////    multi getAt : aKey
////    [
////        int aHashCode := aKey hash_code.
////        
////        ^ $self $getItem int:aHashCode object:aKey; value.
////    ]
////            
////    getAt int:aKey
////    [
////        int aHashCode := 0.
////        aKey readHashCodeTo vint:aHashCode.
////        
////        ^ $self $getItem int:aHashCode object:aKey; value.
////    ]
////        
////    getAt long:aKey
////    [
////        int aHashCode := 0.
////        aKey readHashCodeTo vint:aHashCode.
////        
////        ^ $self $getItem int:aHashCode object:aKey; value.
////    ]
////        
////    getAt real:aKey
////    [
////        int aHashCode := 0.
////        aKey readHashCodeTo vint:aHashCode.
////        
////        ^ $self $getItem int:aHashCode object:aKey; value.
////    ]
////        
////    getAt char:aKey
////    [
////        int aHashCode := 0.
////        aKey readHashCodeTo vint:aHashCode.
////        
////        ^ $self $getItem int:aHashCode object:aKey; value.
////    ]
////        
////    getAt literal:aKey
////    [
////        int aHashCode := 0.
////        aKey readHashCodeTo vint:aHashCode.
////        
////        ^ $self $getItem int:aHashCode object:aKey; value.
////    ]
////        
////    getAt wide:aKey
////    [
////        int aHashCode := 0.
////        aKey readHashCodeTo vint:aHashCode.
////        
////        ^ $self $getItem int:aHashCode object:aKey; value.
////    ]
////
////    bool containsKey(object aKey)
////    [
////        int aHashCode := aKey hash_code.
////        
////        aHashCode := aHashCode && theHashMask.
////        
////        var aHashItem := theHashTable getAt int:aHashCode.
////        if ($nil == aHashItem)
////        [
////            aHashItem := IntArray new int:5.
////            
////            aHashItem write int:0 int:1.
////            
////            theHashTable setAt int:aHashCode object:aHashItem
////        ].
////        
////        intarray aBacket := aHashItem intarray.        
////        int      aLength := 0.
////        aBacket read int:0 vint:aLength.
////        
////        int i := 1.
////        while (i < aLength)
////        [
////            int anItemIndex := 0.
////            aBacket read int:i vint:anItemIndex.
////            
////            var anItem := theList getAt int:anItemIndex.
////            
////            if(aKey equal(anItem key))
////                [ ^ true ].
////            
////            i := i + 1
////        ].        
////        
////        ^ false
////    ]
////
////    /// Returns a dictionary enumerator
////    enumerator => theList.
////    
////    array_list
////        = theList clone.
////
////    /// Clones a dictionary
////    clone 
////    [
////        var aList := Dictionary new.
////
////        enumerator it := theList enumerator.
////        while (it next)
////        [
////            var anItem := it get.
////            
////            aList[anItem key] := anItem value.
////        ].
////        
////        ^ aList.
////    ]
////    
////    clear
////    [
////        theHashTable := Array new int:(theHashTable length).
////        theList clear.
////    ]
//}
