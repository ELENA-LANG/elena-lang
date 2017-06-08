type list_item  :: system'collections'ListItem.
type list       :: system'collections'List.
type array_list :: system'collections'ArrayList.
type dictionary :: system'collections'Dictionary.

//// --- listof ---
//
//#template listof(type)
//{
//    arrayof(type) theArray.
//    vint          theLength.
//    vint          theCapacity.
//
//    implicit array:list
//    [
//        int len := list length.
//        int size := len / 32.
//        size += 1.
//        size *= 32.
//        
//        theLength := len.
//        theCapacity := size.
//        theArray := type(size).
//        
//        int i := 0.
//        while(i < len)
//        [
//            theArray@i := list@i.
//            
//            i += 1.
//        ].
//    ]
//    
//    implicit int:capacity
//    [
//        theCapacity := capacity.
//        theLength := 0.
//        theArray := type(capacity).
//    ]
//
//    $expand
//    [
//        arrayof(type)newArray := type(theCapacity).
//        
//        if (theCapacity <= theLength)
//        [
//            theCapacity += 32.
//            
//            arrayof(type)newArray := type(theCapacity).
//
//            int i := 0.
//            while (i < theLength)
//            [
//                newArray setAt &int:i &type:(theArray getAt &int:i).
//                
//                i += 1.
//            ].
//            
//            theArray := newArray.
//        ].
//    ]
//    
//    /// Returns the object at the specified position
//    type getAt : anIndex
//        = $self getAt &int:(anIndex int).
//
//    /// Returns the object at the specified position
//    type stacksafe getAt &int:index
//    [
//        ifnot(index < theLength)
//            [ OutOfRangeException new raise. ].
//            
//        ^ theArray getAt &int:index.
//    ]
//
//    /// Returns the object at the specified position
//    stacksafe setAt &int:index &type:object
//    [
//        ifnot(index < theLength)
//            [ OutOfRangeException new raise. ].
//        
//        theArray setAt &int:index &type:object.
//    ]
//
//    /// Returns the object at the specified position
//    setAt : anIndex : anObject
//        = $self setAt &int:(anIndex int) &type:anObject.
//
//    append : object
//        <= append &type:object.
//
//    /// Appends a new object to the end of the list
//    append &type:object
//    [
//        $self $expand.
//        
//        theArray setAt &int:theLength &type:object.
//        
//        theLength += 1.
//    ]
//
//    /// Removes an object at the specified position
//    stacksafe remove &int:anIndex
//    [
//        ifnot (anIndex < theLength)
//            [ OutOfRangeException new raise. ].
//        
//        theLength -= 1.
//        
//        int i := anIndex.
//        while(i < theLength)
//        [
//            theArray setAt &int:i &type:(theArray getAt &int:(i + 1)).
//            
//            i += 1.
//        ].
//    ]
//        
//    /// Clears the list
//    clear 
//    [
//        theLength set &int:0.        
//    ]
//    
//    enumerator 
//    [
//        vint index := -1.
//        int length := theLength.
//        
//        ^ Enumerator
//          {
//              get = $owner getAt &int:index.
//              
//              enumerable = $owner.
//              
//              reset [ index := -1. ]
//              
//              next
//              [
//                  index += 1.
//                  
//                  ^ index < length.
//              ]
//          }.
//    ]
//
//    indexer = BaseIndexer
//    {
//        implicit object
//        [
//            theIndex := 0.
//
//            int len := 0.                                    
//            $owner readLength &vint:len.
//            
//            theLength := len.
//        ]
//
//        get
//            = $owner getAt &int:theIndex.
//            
//        set : o
//        [
//            $owner setAt &int:theIndex &type:o.
//        ]                
//    }.
//                        
//    stacksafe outnprop(length) readLength &vint:retVal
//    [
//        retVal := theLength.
//    ]    
//}

// --- ObjectList ---

//#class(listof:object,sealed) ObjectList
//{
//    #constructor(w3) new
//    [
//        theArray := object:32.
//        theLength := 0.
//        theCapacity := 32.
//    ]
//}

// --- ListItem ---

/// A list item
sealed class ListItem
{
    object    theContent.
    list_item theNext.
    
    /// Creates a new item
    constructor new : aValue
    [
        theContent := aValue.
    ]

    /// Returns the item object
    get = theContent.
            
    /// Sets the item object
    set : anObject
    [
        theContent := anObject.
    ]    

    /// Returns the next item
    list_item nextItem = theNext.

    /// Sets the next item
    setNextItem list_item:aNext
    [
        theNext := aNext.
    ]
}

// --- ListEnumerator ---

/// A list enumerator
class ListEnumerator :: Enumerator
{
    list      theList.
    list_item theCurrent.
    
    /// Creates a list enumerator
    constructor new list:aList
    [
        theList := aList.
    ]

    /// Creates a list enumerator
    constructor new : anObject
        <= new list:anObject.

    /// Returns the current object
    get = theCurrent get.
    
    /// Resets the enumerator
    reset
    [
        theCurrent := $nil.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    next
    [
        if ($nil == theCurrent)
            [ theCurrent := theList topItem ];
            [ theCurrent := theCurrent nextItem ].
            
        ^ $nil != theCurrent
    ]
}

// --- ListIndexer ---

/// A list indexer
class ListIndexer :: Indexer
{
    list      theList.
    list_item theCurrent.
    vint      theIndex.
    
    /// Creates a list indexer
    constructor new list:aList
    [
        theList := aList.
        theCurrent := aList topItem.
        theIndex := 0.
    ]

    /// Creates a list indexer
    constructor new : anObject
        <= new list:anObject.
    
    /// Returns false if the index out of range
    available = $nil != theCurrent.

    /// Returns the current member
    get = theCurrent get.
    
    /// Sets the current member
    set : anObject 
    [
        theCurrent set:anObject.
    ]
    
    /// Reads the current index into an output parameter
    readIndexTo vint:anIndex
    [
        anIndex int := theIndex.
    ]
    
    /// Sets the index by specified value
    writeIndex int:anIndex
    [
        int i := theIndex.
        
        if(anIndex < i)
        [
            theCurrent := theList topItem.
            i := 0.
        ].
        
        while (i < anIndex) 
        [
            if ($nil == theCurrent)
                [ OutOfRangeException new; raise. ].
            
            theCurrent := theCurrent nextItem.            
            i := i + 1
        ].
        
        theIndex set int:i.
    ]
}

// --- List ---

/// A list object
sealed class List :: dispatchable(enumerable)
{
    object    theState.
    vint      theCount.
    list_item theTop.
    list_item theLast.

    /// Creates a list
    constructor new
    [
        theState := %empty.
        theCount := 0.
    ]

    /// Returns the top item
    list_item topItem = theTop.

    /// Returns the top object
    first = theTop get.

    /// Returns the last object
    last = theLast get.    

    append empty : anObject
    [
        theCount set int:1.
        theTop := ListItem new:anObject.
        theLast := theTop.
        theState := %active.
    ]

    insert empty : anObject
        <= append empty:anObject.

    append active : anObject
    [
        theCount append int:1.
        
        list_item aNewItem := ListItem new:anObject.
        
        theLast setNextItem list_item:aNewItem.        
        theLast := aNewItem.
    ]

    insert active : anObject
    [
        theCount append int:1.        
        
        list_item aNewItem := ListItem new:anObject.
        aNewItem setNextItem list_item:theTop.        
        theTop := aNewItem.
    ]
    
    /// Returns the object at the specified position
    getAt : anIndex
        = $self getAt int:(anIndex int).

    /// Returns the object at the specified position
    setAt : anIndex : anObject
        = $self setAt int:(anIndex int) object:anObject.

    /// Returns the list item at the specified position
    stacksafe list_item getItemAt int:anIndex
    [
        int       aCurrentIndex := anIndex.
        list_item anItem := theTop.
        
        while (aCurrentIndex > 0)
        [
            anItem := anItem nextItem.
            if ($nil == anItem)
                [ OutOfRangeException new; raise ].
            
            aCurrentIndex := aCurrentIndex - 1
        ].
        
        ^ anItem.
    ]

    /// Returns the object at the specified position
    stacksafe getAt int:anIndex
        = $self getItemAt int:anIndex; get.

    /// Returns the object at the specified position
    stacksafe setAt int:anIndex object:anObject
        = $self getItemAt int:anIndex; set:anObject.

    /// Appends a new object to the end of the list
    append : anObject => theState.

    /// Inserts a new object to the beginning of the list
    insert : anObject => theState.

    insert : anObject at:anIndex
        <= insert int:anIndex object:anObject.

    /// Inserts a new object to the specified position
    insert int:anIndex object:anObject
    [
        if (anIndex == 0)
            [ $self insert:anObject ];
            [
                list_item aNew := ListItem new:anObject.        
                list_item aPrevious := $self getItemAt int:(anIndex - 1).
                list_item aNext := aPrevious nextItem.
                
                aPrevious setNextItem list_item:aNew.
                aNew setNextItem list_item:aNext
            ]
    ]

    /// Removes an object at the specified position
    remove int:anIndex
    [
        if ($nil == theTop)
            [ OutOfRangeException new; raise ].
        
        if (0 == anIndex)
            [
                theTop := theTop nextItem.
                if ($nil == theTop)
                    [ theLast := $nil. theState := %empty ]
            ];
            [
                list_item aPrevious := $self getItemAt int:(anIndex - 1).
                list_item aNext := aPrevious nextItem.
                
                aPrevious setNextItem list_item:(aNext nextItem).
                
                if(aNext equal reference:theLast)
                    [ theLast := aPrevious ].
            ].
            
        theCount reduce int:1.
    ]
        
    remove at:anIndex
        <= remove int:anIndex.

    /// obsolete        
    remove index:anIndex
        <= remove int:anIndex.
        
    int length :: outnprop(readLengthTo).
        
    /// Reads the number of list item to the output parameter
    stacksafe readLengthTo vint:aLength
    [
        aLength int := theCount.
    ]
    
    /// Clears the list
    clear 
    [
        theCount int := 0.
        
        theState := %empty.
        theTop := $nil.
        theLast := $nil.
    ]

    /// Returns the list enumerator
    enumerator = ListEnumerator new list:$self.

    /// Returns the list indexer
    indexer = ListIndexer new list:$self.
    
    /// Clones the list
    clone 
    [
        object aList := List new.
        
        list_item anItem := theTop.        
        while ($nil != anItem)
        [
            aList append:(anItem get).
            
            anItem := anItem nextItem
        ].
    ]
}

// --- ArrayList ---

/// An array list object
sealed class ArrayList :: dispatchable(enumerable)
{
    array theArray.
    vint  theLength.
    vint  theCapacity.
    
    /// Creates the list
    constructor new
    [
        theCapacity := 32.
        theLength := 0.
        theArray := Array new int:32.
    ]
    
    /// Creates the list
    constructor new : param
        <= new int:(param cast:%eval to:
                        { 
                            int : n = n. 
                            
                            array : a = a length.
                            
                            generic : n = 32. 
                        });
    [
        $self append array:(param cast:%eval to:{ array : a = a. generic : o = $nil. }).
    ]
    
    constructor new int:aCapacity
    [
        theCapacity := aCapacity.
        theLength := 0.
        theArray := Array new int:aCapacity.
    ]
    
    /// Creates the list
    stacksafe constructor $new array:anArray int:aLength
    [
        int capacity := anArray length.
        
        theCapacity := capacity.
        theLength := aLength.
        
        theArray := anArray clone.
    ]
    
    int length :: outnprop(readLengthTo).
    
    int capacity :: outnprop(readCapacityTo).
    
    /// Returns the object at the specified position
    getAt : anIndex
        = $self getAt int:anIndex.

    /// Returns the object at the specified position
    stacksafe getAt int:anIndex
    [
        ifnot (theLength > anIndex)
            [ OutOfRangeException new; raise ].

        ^ theArray getAt int:anIndex
    ]
    
    /// Returns the object at the specified position
    setAt : anIndex : anObject
        = $self setAt int:anIndex object:anObject.

    /// Returns the object at the specified position
    stacksafe setAt int:anIndex object:anObject
    [
        ifnot (theLength > anIndex)
            [ OutOfRangeException new; raise ].
        
        theArray setAt int:anIndex object:anObject
    ]
    
    $expand
    [
        if (theCapacity <= theLength)
        [
            theCapacity append int:32.
            
            array aNewArray := Array new int:theCapacity.
            int   i := 0.
            while (i < theLength)
            [
                aNewArray setAt int:i object:(theArray getAt int:i).
                
                i := i + 1
            ].
            
            theArray := aNewArray
        ]
    ]
    
    append array:subarray
    [
        if ($nil != subarray)
        [
            int len := subarray length.
            int i := 0.
            while (i < len)
            [
                $self append object:(subarray getAt int:i).
                i := i + 1
            ]
        ]     
    ]
        
    /// Appends a new object to the end of the list
    append object:anObject
    [
        $self $expand.
        
        theArray setAt int:theLength object:anObject.
        
        theLength append int:1.
    ]
    
    append : object
        <= append object:object.
    
    /// Inserts a new object to the specified position
    insert int:anIndex object:anObject
    [
        if (anIndex >= theLength)
            [ OutOfRangeException new; raise ].
        
        $self $expand.
        
        int i := theLength.
        while (i > anIndex)
        [
            theArray setAt int:i object:(theArray getAt int:(i - 1)).
            
            i := i - 1
        ].
        
        theArray setAt int:anIndex object:anObject
    ]    
    
    /// Inserts a new object to the beginning of the list
    insert : anObject
        <= insert int:0 object:anObject.

    insert : anObject at:anIndex
        <= insert int:anIndex object:anObject.
        
    /// Removes an object at the specified position
    stacksafe remove int:anIndex
    [
        if (anIndex >= theLength)
            [ OutOfRangeException new; raise ].
        
        theLength reduce int:1.
        
        int i := anIndex.
        while (i < theLength)
        [
            theArray setAt int:i object:(theArray getAt int:(i + 1)).
            
            i := i + 1
        ]
    ]
        
    remove index:anIndex
        <= remove int:anIndex.

    remove at:anIndex
        <= remove int:anIndex.

    /// Returns itself
    array_list = $self.
    
    /// Reads the number of list item to the output parameter
    stacksafe readLengthTo vint:aLength
    [
        aLength int := theLength.
    ]
    
    stacksafe readCapacityTo vint:aCapacity
    [
        aCapacity int := theCapacity.
    ]
    
    /// Clears the list
    clear 
    [
        theLength int := 0
    ]
    
    /// Returns the top object
    first = theArray getAt int:0.

    /// Returns the last object
    last
    [
        int len := theLength.
        
        ^ theArray getAt int:(len - 1)
    ]
    
    /// Returns the list enumerator
    enumerator = ArrayListEnumerator new array_list:$self.
    
    /// Returns the list indexer9
    indexer = ArrayListIndexer new array_list:$self.
    
    /// Returns a list copy
    array_list clone
        = ArrayList $new array:theArray int:theLength.
        
    array
        = Array new int:0 int:theLength array:theArray.
        
    get
        <= array.
}

// --- ArrayListEnumerator ---

/// A list enumerator
class ArrayListEnumerator :: Enumerator
{
    array_list theList.
    vint       theIndex.
    
    /// Creates a list enumerator
    constructor new array_list:aList
    [
        theList := aList.
        theIndex := -1.
    ]

    /// Creates a list enumerator
    constructor new : anObject
        <= new array_list:anObject.

    /// Returns the current object
    get = theList getAt int:theIndex.
    
    /// Resets the enumerator
    reset
    [
        theIndex int := -1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    next
    [
        theIndex append int:1.
        
        int aLength := theList length.
        
        ^ (aLength > theIndex)
    ]
}

// --- ArrayListIndexer ---

/// A list indexer
class ArrayListIndexer :: Indexer
{
    array_list theList.
    vint       theIndex.
    
    /// Creates a list indexer
    constructor new array_list:aList
    [
        theList := aList.
        theIndex := 0.
    ]

    /// Creates a list indexer
    constructor new : anObject
        <= new array_list:anObject.
    
    /// Returns false if the index out of range
    available
    [
        int aLength := theList length.
        
        ^ (aLength > theIndex)
    ]

    /// Returns the current member
    get = theList getAt int:theIndex.
    
    /// Sets the current member
    set : anObject 
    [
        theList setAt int:theIndex object:anObject.
    ]
    
    /// Reads the current index into an output parameter
    readIndexTo vint:anIndex
    [
        anIndex int := theIndex.
    ]
    
    /// Sets the index by specified value
    writeIndex int:anIndex
    [
        theIndex set int:anIndex.
    ]
}

// --- Dictionary ---

/// Dictionary class
sealed  class Dictionary :: dispatchable(enumerable)
{
    array_list theList.
    array      theHashTable.
    int        theHashMask.
    
    object     theDefault.

    /// Creates a dictionary
    constructor new
        <= new int:32 int:32 object:$nil.

    constructor new default:aDefault
        <= new int:32 int:32 object:aDefault.

    constructor new:aCapacity default:aDefault
        <= new int:(aCapacity int) int:32 object:aDefault.

    /// Creates a dictionary
    constructor new int:aCapacity int:aHashLength object:aDefault
    [
        int size := 32.
        while (size < aHashLength)
        [
            size := size << 1.
        ].
        
        size reduce int:1.
        theHashMask := IntNumber new int:size.
        
        theHashTable := Array new int:aHashLength.
        theList := ArrayList new int:aCapacity.
        
        theDefault := aDefault.
    ]
    
    /// Gets a dictionary item with the specified key
    getAt : aKey
        = aKey cast:%getAt to:$self.
        
    /// Sets a dictionary item with the specified key
    setAt : aKey : anObject
    [
        int aHashCode := aKey hash_code.
        
        $self $getItem int:aHashCode key:aKey; set value:anObject.
    ]
        
    stacksafe $getItem int:aCode key:aKey
    [
        int aHashCode := aCode && theHashMask.
        
        var aHashItem := theHashTable getAt int:aHashCode.
        if ($nil == aHashItem)
        [
            aHashItem := IntArray new int:5.
            
            aHashItem write int:0 int:1.
            
            theHashTable setAt int:aHashCode object:aHashItem
        ].
        
        intarray aBacket := aHashItem intarray.        
        int      aLength := 0.
        aBacket read int:0 vint:aLength.
        
        int i := 1.
        while (i < aLength)
        [
            int anItemIndex := 0.
            aBacket read int:i vint:anItemIndex.
            
            var anItem := theList getAt int:anItemIndex.
            
            if(aKey safeEqual:(anItem key))
                [ ^ anItem ].
            
            i := i + 1
        ].
        
        var aNewItem := KeyValue new key:aKey value:theDefault.
        
        theList readLengthTo vint:i.
        theList append:aNewItem.
        
        // Extend a backet
        int aCapacity := 0.
        aBacket readLengthTo vint:aCapacity.
        if (aCapacity == aLength)
            [
                intarray aNewBacket := IntArray new int:(aCapacity + 5).
                aNewBacket write int:0 int:aCapacity intarray:aBacket.

                aBacket := aNewBacket.
                theHashTable setAt int:aHashCode object:aNewBacket.
            ].
            
        aBacket write int:aLength int:i.
            
        aLength := aLength + 1.
        aBacket write int:0 int:aLength.
        
        ^ aNewItem.
    ]    
        
    generic getAt : aKey
    [
        int aHashCode := aKey hash_code.
        
        ^ $self $getItem int:aHashCode key:aKey; value.
    ]
            
    getAt int:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode key:aKey; value.
    ]
        
    getAt long:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode key:aKey; value.
    ]
        
    getAt real:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode key:aKey; value.
    ]
        
    getAt char:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode key:aKey; value.
    ]
        
    getAt literal:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode key:aKey; value.
    ]
        
    getAt wide:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode key:aKey; value.
    ]

    /// Returns a dictionary enumerator
    enumerator => theList.
    
    array_list
        = theList clone.

    /// Clones a dictionary
    clone 
    [
        var aList := Dictionary new.

        enumerator it := theList enumerator.
        while (it next)
        [
            var anItem := it get.
            
            aList@(anItem key) := anItem value.
        ].
        
        ^ aList.
    ]
    
    clear
    [
        theHashTable := Array new int:(theHashTable length).
        theList clear.
    ]
}
