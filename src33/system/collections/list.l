type list_item  = system'collections'ListItem.
type list       = system'collections'List.
type array_list = system'collections'ArrayList.
type dictionary = system'collections'Dictionary.

class BaseEnumerable
{
    // should be overridden
    enumerator = $nil.
}

// --- listof ---

sealed template<BaseEnumerable> list :: type
{
    array<type> theArray.
    vint        theLength.
    vint        theCapacity.

    implicit array:list
    [
        int len := list length.
        int size := len / 32.
        size := size + 1.
        size := size * 32.
        
        theLength := len.
        theCapacity := size.
        theArray := type<>(size).
        
        int i := 0.
        while(i < len)
        [
            theArray[i] := list[i].
            
            i := i + 1.
        ].
    ]
    
    implicit int:capacity
    [
        theCapacity := capacity.
        theLength := 0.
        theArray := type<>(capacity).
    ]
    
    implicit
    [
        theCapacity := 16.
        theLength := 0.
        theArray := type<>(16).
    ]
    
    int rvprop length :: readLengthTo.

    $expand
    [
        if (theCapacity <= theLength)
        [
            theCapacity append int:32.
            
            array<type> newArray := type<>(theCapacity).

            int i := 0.
            while (i < theLength)
            [
                newArray setAt int:i type:(theArray getAt int:i).
                
                i := i + 1.
            ].
            
            theArray := newArray.
        ].
    ]
    
    /// Returns the object at the specified position
    type getAt : anIndex
        = $self getAt int:anIndex.

    /// Returns the object at the specified position
    type stacksafe getAt int:index
    [
        ifnot(index < theLength)
            [ OutOfRangeException new; raise ].
            
        ^ theArray getAt int:index
    ]

    /// Returns the object at the specified position
    stacksafe setAt int:index type:object
    [
        ifnot(index < theLength)
            [ OutOfRangeException new; raise ].
        
        theArray setAt int:index type:object.
    ]

    /// Returns the object at the specified position
    setAt : anIndex : anObject
        = $self setAt int:anIndex type:anObject.

    append : object
        <= append type:object.

    /// Appends a new object to the end of the list
    append type:object
    [
        $self $expand.
        
        theArray setAt int:theLength type:object.
        
        theLength append int:1.
    ]

    /// Removes an object at the specified position
    stacksafe remove int:anIndex
    [
        ifnot (anIndex < theLength)
            [ OutOfRangeException new; raise ].
        
        theLength reduce int:1.
        
        int i := anIndex.
        while(i < theLength)
        [
            theArray setAt int:i type:(theArray getAt int:(i + 1)).
            
            i := i + 1
        ]
    ]
        
    /// Returns the top object
    type first = theArray getAt int:0.
    
    /// Clears the list
    clear 
    [
        theLength int := 0.        
    ]
    
    stacksafe trim(IntNumber pos)
    [
        if (pos > theLength)
            [ OutOfRangeException new; raise ].
        
        theLength int := pos.
    ]
    
    enumerator 
    [
        vint index := -1.
        int length := theLength.
        
        ^ Enumerator::
          {
              get = $self getAt int:index.
              
              enumerable = $self.
              
              reset [ index int := -1. ]
              
              bool next
              [
                  index append int:1.
                  
                  ^ index < length
              ]
          }.
    ]

    indexer = BaseIndexer::
    {
        implicit
        [
            theIndex := 0.

            int len := 0.                                    
            $self readLengthTo vint:len.
            
            theBaseIndexerLength := len.
        ]

        get
            = $self getAt int:theIndex.
            
        set : o
        [
            $self setAt int:theIndex type:o.
        ]                
    }.
                       
    array
    [
        array a := Array new(theLength).
        int i := 0.
        while (i < theLength)
        [
            a[i] := theArray[i].
            
            i += 1
        ].
        
        ^ a
    ]                       
                       
    stacksafe readLengthTo vint:retVal
    [
        retVal int := theLength.
    ]    
}

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
    bool next
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
    bool available = $nil != theCurrent.

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
sealed class List :: BaseEnumerable
{
    vint      theCount.
    list_item theTop.
    list_item theLast.

    /// Creates a list
    constructor new
    [
        theCount := 0.
    ]

    constructor new array:l
    [
        theCount := 0.
        
        var it := l enumerator.
        while (it next)
        [
            $self append(it get)
        ].
    ]

    /// Returns the top item
    list_item topItem = theTop.

    /// Returns the top object
    first = theTop get.

    /// Returns the last object
    last = theLast get.    

    append : anObject
    [
        theCount append int:1.
        if($nil == theTop)
        [
            theTop := ListItem new:anObject.
            theLast := theTop.
        ];
        [
            list_item aNewItem := ListItem new:anObject.
            
            theLast setNextItem list_item:aNewItem.        
            theLast := aNewItem.
        ].

    ]

    insert : anObject
    [
        theCount append int:1.        
        
        list_item aNewItem := ListItem new:anObject.
        aNewItem setNextItem list_item:theTop.        
        theTop := aNewItem.
    ]
    
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
                    [ theLast := $nil. ]
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

    int rvprop length :: readLengthTo.
        
    /// Reads the number of list item to the output parameter
    stacksafe readLengthTo vint:aLength
    [
        aLength int := theCount.
    ]
    
    /// Clears the list
    clear 
    [
        theCount int := 0.
        
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
sealed class ArrayList :: BaseIndexable
{
    array theArray.
    vint  theLength.
    vint  theCapacity.
    
    /// Creates the list
    multi constructor new
    [
        theCapacity := 32.
        theLength := 0.
        theArray := Array new int:32.
    ]

    constructor new int:aCapacity
    [
        theCapacity := aCapacity.
        theLength := 0.
        theArray := Array new int:aCapacity.
    ]
    
    constructor new array:a
        <= $new array:a int:(a length).
    
    /// Creates the list
    stacksafe constructor $new array:anArray int:aLength
    [
        int capacity := anArray length.
        
        theCapacity := capacity.
        theLength := aLength.
        
        theArray := anArray clone.
    ]
    
    int rvprop length :: readLengthTo.
    
    int rvprop capacity :: readCapacityTo.
    
    /// Returns the object at the specified position
    stacksafe getAt int:anIndex
    [
        ifnot (theLength > anIndex)
            [ OutOfRangeException new; raise ].

        ^ theArray getAt int:anIndex
    ]
    
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
    
    /// Appends a new object to the end of the list
    append:anObject
    [
        $self $expand.
        
        theArray setAt int:theLength object:anObject.
        
        theLength append int:1.
    ]
    
    multi appendRange : subarray
    [
        if ($nil != subarray)
        [
            int len := subarray length.
            int i := 0.
            while (i < len)
            [
                $self append(subarray getAt int:i).
                i := i + 1
            ]
        ]
    ]

    appendRange(BaseIndexable indexable)
    [
        if ($nil != indexable)
        [
            int len := indexable length.
            indexer it := indexable indexer.
            
            int i := 0.
            while (it available)
            [                
                $self append(it get).
                
                i += 1.
                it writeIndex int:i.
            ]
        ]
    ]        
                
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
        
    remove at:anIndex
        <= remove int:anIndex.
    
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
    
    stacksafe trim(IntNumber pos)
    [
        if (pos > theLength)
            [ OutOfRangeException new; raise ].
        
        theLength int := pos.
    ]
    
    /// Returns the top object
    first = theArray getAt int:0.

    /// Returns the last object
    last
    [
        int len := theLength.
        
        ^ theArray getAt int(len - 1)
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

    value 
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
    bool next
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
    bool available
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
sealed  class Dictionary :: BaseEnumerable
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
        
        size := size - 1.
        theHashMask := IntNumber new int:size.
        
        theHashTable := Array new int:aHashLength.
        theList := ArrayList new int:aCapacity.
        
        theDefault := aDefault.
    ]
    
    setAt int:aKey object:anObject
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        $self $getItem int:aHashCode object:aKey; set value:anObject
    ]
        
    setAt long:aKey object:anObject
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        $self $getItem int:aHashCode object:aKey; set value:anObject
    ]
        
    setAt real:aKey object:anObject
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        $self $getItem int:aHashCode object:aKey; set value:anObject
    ]
        
    setAt char:aKey object:anObject
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        $self $getItem int:aHashCode object:aKey; set value:anObject
    ]
        
    setAt literal:aKey object:anObject
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        $self $getItem int:aHashCode object:aKey; set value:anObject
    ]
        
    setAt wide:aKey object:anObject
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        $self $getItem int:aHashCode object:aKey; set value:anObject.
    ]
    
    /// Sets a dictionary item with the specified key
    multi setAt : aKey : anObject
    [
        int aHashCode := aKey hash_code.
        
        $self $getItem int:aHashCode object:aKey; set value:anObject.
    ]
        
    stacksafe $getItem int:aCode object:aKey
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
            
            if(aKey equal(anItem key))
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
        
    multi getAt : aKey
    [
        int aHashCode := aKey hash_code.
        
        ^ $self $getItem int:aHashCode object:aKey; value.
    ]
            
    getAt int:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode object:aKey; value.
    ]
        
    getAt long:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode object:aKey; value.
    ]
        
    getAt real:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode object:aKey; value.
    ]
        
    getAt char:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode object:aKey; value.
    ]
        
    getAt literal:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode object:aKey; value.
    ]
        
    getAt wide:aKey
    [
        int aHashCode := 0.
        aKey readHashCodeTo vint:aHashCode.
        
        ^ $self $getItem int:aHashCode object:aKey; value.
    ]

    bool containsKey(object aKey)
    [
        int aHashCode := aKey hash_code.
        
        aHashCode := aHashCode && theHashMask.
        
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
            
            if(aKey equal(anItem key))
                [ ^ true ].
            
            i := i + 1
        ].        
        
        ^ false
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
            
            aList[anItem key] := anItem value.
        ].
        
        ^ aList.
    ]
    
    clear
    [
        theHashTable := Array new int:(theHashTable length).
        theList clear.
    ]
}
