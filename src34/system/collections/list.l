// --- listof ---

public sealed List<type> :: Enumerable<type>
{
    Array<type> theArray.
    ref<int>    theLength.
    ref<int>    theCapacity.

    cast eval(Array list)
    [
        int len := list length.
        int size := len / 32.
        size := size + 1.
        size := size * 32.
        
        theLength := len.
        theCapacity := size.
        theArray := V<type>(size).
        
        int i := 0.
        while(i < len)
        [
            theArray[i] := list[i].
            
            i := i + 1.
        ].
    ]
    
    cast eval(int capacity)
    [
        theCapacity := capacity.
        theLength := 0.
        theArray := V<type>(capacity).
    ]
    
    constructor default
    [
        theCapacity := 16.
        theLength := 0.
        theArray := V<type>(16).
    ]
    
    int rvprop length :: readLengthTo.

    private expand
    [
        if (theCapacity <= theLength)
        [
            theCapacity append(32).
            
            Array<type> newArray := V<type>(theCapacity).

            int i := 0.
            while (i < theLength)
            [
                newArray setAt(i, theArray getAt(i)).
                
                i := i + 1.
            ].
            
            theArray := newArray.
        ].
    ]
    
    /// Returns the object at the specified position
    type getAt : anIndex
        = self getAt(T<int>(anIndex)).

    /// Returns the object at the specified position
    type stacksafe getAt(int index)
    [
        ifnot(index < theLength)
            [ OutOfRangeException new; raise ].
            
        ^ theArray getAt(index)
    ]

    /// Returns the object at the specified position
    stacksafe setAt(int index, type object)
    [
        ifnot(index < theLength)
            [ OutOfRangeException new; raise ].
        
        theArray setAt(index, object).
    ]

    /// Returns the object at the specified position
    setAt(object anIndex, object anObject)
        = self setAt(T<int>(anIndex), T<type>(anObject)).

    /// Appends a new object to the end of the list
    append(type object)
    [
        self expand.
        
        theArray setAt(T<int>(theLength), object).
        
        theLength append(1).
    ]

    append : object
        <= append(T<type>(object)).

    /// Removes an object at the specified position
    stacksafe remove(int anIndex)
    [
        ifnot (anIndex < theLength)
            [ OutOfRangeException new; raise ].
        
        theLength reduce(1).
        
        int i := anIndex.
        while(i < theLength)
        [
            theArray setAt(i, theArray getAt(i + 1)).
            
            i := i + 1
        ]
    ]
        
    /// Returns the top object
    type first = theArray getAt(0).
    
    /// Clears the list
    clear 
    [
        theLength value := 0.        
    ]
    
    stacksafe trim(IntNumber pos)
    [
        if (pos > theLength)
            [ OutOfRangeException new; raise ].
        
        theLength value := pos.
    ]
    
    Enumerator<type> enumerator = BaseEnumerator<type>::
    {
        @theIndex := -1.
        
        @theLength := self length.
        
        type get = self getAt(@theIndex).
        
        enumerable = self.
        
        reset
        [
            @theIndex := -1
        ]
    }.

    Indexer<type> indexer = BaseIndexer<type>::
    {
        @theIndex := 0.
        
        @theLength := self length.
        
        type get
            = self getAt(theIndex).
            
        set(type o)
        [
            self setAt(theIndex, o).
        ]
    }.    
        
    cast T<Array>
        = Array new(0, T<int>(theLength), theArray).

    value 
        = Array new(0, T<int>(theLength), theArray).
                       
    stacksafe readLengthTo(ref<int> retVal)
    [
        theLength readValueTo(retVal).
    ]    
}

// --- ListItem ---

/// A list item
sealed class ListItem
{
    object      theContent.
    T<ListItem> theNext.
    
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
    T<ListItem> nextItem = theNext.

    /// Sets the next item
    setNextItem(ListItem aNext)
    [
        theNext := aNext.
    ]
}

// --- ListEnumerator ---

/// A list enumerator
class ListEnumerator :: Enumerator
{
    T<List>     theList.
    T<ListItem> theCurrent.
    
    /// Creates a list enumerator
    constructor new(List aList)
    [
        theList := aList.
    ]

    /// Creates a list enumerator
    constructor new : anObject
        <= new(T<List>(anObject)).

    /// Returns the current object
    get = theCurrent get.
    
    /// Resets the enumerator
    reset
    [
        theCurrent := nil.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next
    [
        if (nil == theCurrent)
            [ theCurrent := theList topItem ];
            [ theCurrent := theCurrent nextItem ].
            
        ^ nil != theCurrent
    ]
    
    enumerable = theList.
}

// --- ListIndexer ---

/// A list indexer
class ListIndexer :: Indexer
{
    T<List>     theList.
    T<ListItem> theCurrent.
    ref<int>    theIndex.
    
    /// Creates a list indexer
    constructor new(List aList)
    [
        theList := aList.
        theCurrent := aList topItem.
        theIndex := 0.
    ]

    /// Creates a list indexer
    constructor new : anObject
        <= new(T<List>(anObject)).
    
    /// Returns false if the index out of range
    bool available = nil != theCurrent.

    /// Returns the current member
    get = theCurrent get.
    
    /// Sets the current member
    set : anObject 
    [
        theCurrent set:anObject.
    ]
    
    /// Reads the current index into an output parameter
    readIndexTo(ref<int> anIndex)
    [
        theIndex readValueTo(anIndex).
    ]
    
    /// Sets the index by specified value
    writeIndex(int anIndex)
    [
        int i := theIndex.
        
        if(anIndex < i)
        [
            theCurrent := theList topItem.
            i := 0.
        ].
        
        while (i < anIndex) 
        [
            if (nil == theCurrent)
                [ OutOfRangeException new; raise. ].
            
            theCurrent := theCurrent nextItem.            
            i := i + 1
        ].
        
        theIndex value := i.
    ]
    
    stacksafe appendIndex(int anIndex)
    [
        int i := theIndex.
        i := i + anIndex.
        
        self writeIndex(i).
    ]
    
    stacksafe readLengthTo(ref<int> retVal)
    [
        theList readLengthTo(retVal).
    ]
}

// --- List ---

/// A list object
public sealed class List :: Enumerable
{
    ref<int>    theCount.
    T<ListItem> theTop.
    T<ListItem> theLast.

    /// Creates a list
    constructor new
    [
        theCount := 0.
    ]

    constructor new(Array l)
    [
        theCount := 0.
        
        auto it := l enumerator.
        while (it next)
        [
            self append(it get)
        ].
    ]

    /// Returns the top item
    internal T<ListItem> topItem = theTop.

    /// Returns the top object
    first = theTop get.

    /// Returns the last object
    last = theLast get.    

    append : anObject
    [
        theCount append(1).
        if(nil == theTop)
        [
            theTop := ListItem new:anObject.
            theLast := theTop.
        ];
        [
            auto aNewItem := ListItem new:anObject.
            
            theLast setNextItem(aNewItem).
            theLast := aNewItem.
        ].

    ]

    insert : anObject
    [
        theCount append(1).
        
        auto aNewItem := ListItem new:anObject.
        aNewItem setNextItem(theTop).
        theTop := aNewItem.
    ]
    
    /// Returns the list item at the specified position
    internal stacksafe T<ListItem> getItemAt(int anIndex)
    [
        int         aCurrentIndex := anIndex.
        T<ListItem> anItem := theTop.
        
        while (aCurrentIndex > 0)
        [
            anItem := anItem nextItem.
            if (nil == anItem)
                [ OutOfRangeException new; raise ].
            
            aCurrentIndex := aCurrentIndex - 1
        ].
        
        ^ anItem.
    ]

    /// Returns the object at the specified position
    stacksafe getAt(int anIndex)
        = self getItemAt(anIndex); get.

    /// Returns the object at the specified position
    stacksafe setAt(int anIndex, object anObject)
        = self getItemAt(anIndex); set:anObject.

    insert : anObject at:anIndex
        <= insert(anIndex, anObject).

    /// Inserts a new object to the specified position
    insert(int anIndex, object anObject)
    [
        if (anIndex == 0)
            [ self insert:anObject ];
            [
                T<ListItem> aNew := ListItem new:anObject.        
                T<ListItem> aPrevious := self getItemAt(anIndex - 1).
                T<ListItem> aNext := aPrevious nextItem.
                
                aPrevious setNextItem(aNew).
                aNew setNextItem(aNext)
            ]
    ]

    /// Removes an object at the specified position
    remove(int anIndex)
    [
        if (nil == theTop)
            [ OutOfRangeException new; raise ].
        
        if (0 == anIndex)
            [
                theTop := theTop nextItem.
                if (nil == theTop)
                    [ theLast := nil. ]
            ];
            [
                T<ListItem> aPrevious := self getItemAt(anIndex - 1).
                T<ListItem> aNext := aPrevious nextItem.
                
                aPrevious setNextItem(aNext nextItem).
                
                if(aNext equalReference:theLast)
                    [ theLast := aPrevious ].
            ].
            
        theCount reduce(1).
    ]
        
    removeAt:anIndex
        <= remove(T<int>(anIndex)).

    int rvprop length :: readLengthTo.
        
    /// Reads the number of list item to the output parameter
    stacksafe readLengthTo(ref<int> aLength)
    [
        theCount readValueTo(aLength).
    ]
    
    /// Clears the list
    clear 
    [
        theCount value := 0.
        
        theTop := nil.
        theLast := nil.
    ]

    /// Returns the list enumerator
    T<Enumerator> enumerator = ListEnumerator new(self).

    /// Returns the list indexer
    T<Indexer> indexer = ListIndexer new(self).
    
    /// Clones the list
    T<List> clone 
    [
        auto aList := List new.
        
        T<ListItem> anItem := theTop.        
        while (nil != anItem)
        [
            aList append(anItem get).
            
            anItem := anItem nextItem
        ].
        
        ^ aList
    ]
}

// --- ArrayList ---

/// An array list object
public sealed class ArrayList :: Indexable//, interface<Enumerable>
{
    T<Array> theArray.
    ref<int> theLength.
    ref<int> theCapacity.

    /// Creates the list
    constructor new
    [
        theCapacity := 32.
        theLength := 0.
        theArray := Array new(32).
    ]

    constructor new(int aCapacity)
    [
        theCapacity := aCapacity.
        theLength := 0.
        theArray := Array new(aCapacity).
    ]
    
    constructor new(Array a)
        <= fromArray(a, a length).
    
    /// Creates the list
    stacksafe internal constructor fromArray(Array anArray, int aLength)
    [
        int capacity := anArray length.
        
        theCapacity := capacity.
        theLength := aLength.
        
        theArray := anArray clone.
    ]
    
    int rvprop length :: readLengthTo.
    
    int rvprop capacity :: readCapacityTo.
    
    /// Returns the object at the specified position
    stacksafe getAt(int anIndex)
    [
        ifnot (theLength > anIndex)
            [ OutOfRangeException new; raise ].

        ^ theArray getAt(anIndex)
    ]
    
    /// Returns the object at the specified position
    stacksafe setAt(int anIndex, object anObject)
    [
        ifnot (theLength > anIndex)
            [ OutOfRangeException new; raise ].
        
        theArray setAt(anIndex, anObject)
    ]
    
    private expand
    [
        if (theCapacity <= theLength)
        [
            theCapacity append(32).
            
            T<Array> aNewArray := Array new(theCapacity).
            int   i := 0.
            while (i < theLength)
            [
                aNewArray setAt(i, theArray getAt(i)).
                
                i := i + 1
            ].
            
            theArray := aNewArray
        ]
    ]
    
    /// Appends a new object to the end of the list
    append:anObject
    [
        self expand.
        
        theArray setAt(theLength, anObject).
        
        theLength append(1).
    ]
    
    appendRange : subarray
    [
        if (nil != subarray)
        [
            int len := subarray length.
            int i := 0.
            while (i < len)
            [
                self append(subarray getAt(i)).
                i := i + 1
            ]
        ]
    ]

    appendRange(Indexable indexable)
    [
        if (nil != indexable)
        [
            int len := indexable length.
            T<Indexer> it := indexable indexer.
            
            int i := 0.
            while (it available)
            [                
                self append(it get).
                
                i += 1.
                it writeIndex(i).
            ]
        ]
    ]        
                
    /// Inserts a new object to the specified position
    insert(int anIndex, object anObject)
    [
        if (anIndex >= theLength)
            [ OutOfRangeException new; raise ].
        
        self expand.
        
        int i := theLength.
        while (i > anIndex)
        [
            theArray setAt(i, theArray getAt(i - 1)).
            
            i := i - 1
        ].
        
        theArray setAt(anIndex, anObject)
    ]    
    
    /// Inserts a new object to the beginning of the list
    insert : anObject
        <= insert(0, anObject).

    insert : anObject at:anIndex
        <= insert(T<int>(anIndex), anObject).
        
    /// Removes an object at the specified position
    stacksafe remove(int anIndex)
    [
        if (anIndex >= theLength)
            [ OutOfRangeException new; raise ].
        
        theLength reduce(1).
        
        int i := anIndex.
        while (i < theLength)
        [
            theArray setAt(i, theArray getAt(i + 1)).
            
            i := i + 1
        ]
    ]
        
    removeAt:anIndex
        <= remove(T<int>(anIndex)).
    
    /// Reads the number of list item to the output parameter
    stacksafe readLengthTo(ref<int> aLength)
    [
        theLength readValueTo(aLength).
    ]
    
    stacksafe readCapacityTo(ref<int> aCapacity)
    [
        aCapacity value := theCapacity.
    ]
    
    /// Clears the list
    clear 
    [
        theLength value := 0
    ]
    
    stacksafe trim(int pos)
    [
        if (pos > theLength)
            [ OutOfRangeException new; raise ].
        
        theLength value := pos.
    ]
    
    /// Returns the top object
    first = theArray getAt(0).

    /// Returns the last object
    last
    [
        int len := theLength.
        
        ^ theArray getAt(len - 1)
    ]
    
    /// Returns the list enumerator
    enumerator = ArrayListEnumerator new(self).
    
    /// Returns the list indexer9
    T<Indexer> indexer = ArrayListIndexer new(self).
    
    /// Returns a list copy
    T<ArrayList> clone
        = ArrayList fromArray(theArray, T<int>(theLength)).
        
    cast T<Array>
        = Array new(0, T<int>(theLength), theArray).

    value 
        = Array new(0, T<int>(theLength), theArray).
}

// --- ArrayListEnumerator ---

/// A list enumerator
class ArrayListEnumerator :: Enumerator
{
    T<ArrayList> theList.
    ref<int>     theIndex.
    
    /// Creates a list enumerator
    constructor new(ArrayList aList)
    [
        theList := aList.
        theIndex := -1.
    ]

    /// Creates a list enumerator
    constructor new : anObject
        <= new(T<ArrayList>(anObject)).

    /// Returns the current object
    get = theList getAt(T<int>(theIndex)).
    
    /// Resets the enumerator
    reset
    [
        theIndex value := -1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next
    [
        theIndex append(1).
        
        int aLength := theList length.
        
        ^ (aLength > theIndex)
    ]
    
    enumerable = theList.
}

// --- ArrayListIndexer ---

/// A list indexer
public class ArrayListIndexer :: Indexer
{
    T<ArrayList> theList.
    ref<int>     theIndex.
    
    /// Creates a list indexer
    constructor new(ArrayList aList)
    [
        theList := aList.
        theIndex := 0.
    ]

//    /// Creates a list indexer
//    constructor new : anObject
//        <= new array_list:anObject.
    
    /// Returns false if the index out of range
    bool available
    [
        int aLength := theList length.
        
        ^ (aLength > theIndex)
    ]

    /// Returns the current member
    get
    [
        int index := theIndex value.
        
        ^ theList getAt(index).
    ]
    
    /// Sets the current member
    set : anObject 
    [
        int index := theIndex value.
        
        theList setAt(index, anObject).
    ]
    
    /// Reads the current index into an output parameter
    readIndexTo(ref<int> anIndex)
    [
        theIndex readValueTo(anIndex).
    ]
    
    appendIndex(int anIndex)
    [ 
        theIndex append(anIndex).
    ] 
    
    /// Sets the index by specified value
    writeIndex(int anIndex)
    [
        theIndex value := anIndex.
    ]
    
    readLengthTo(ref<int> retVal)
    [
        theList readLengthTo(retVal).
    ]
}

// --- Dictionary ---

/// Dictionary class
public sealed  class Dictionary :: Enumerable
{
    T<ArrayList> theList.
    T<Array>     theHashTable.
    int          theHashMask.
    
    object       theDefault.

    /// Creates a dictionary
    constructor new
        <= new(32, 32, nil).

    constructor new(object aDefault)
        <= new(32, 32, aDefault).

    constructor new(int aCapacity, object aDefault)
        <= new(aCapacity, 32, aDefault).

    /// Creates a dictionary
    constructor new(int aCapacity, int aHashLength, object aDefault)
    [
        int size := 32.
        while (size < aHashLength)
        [
            size := size << 1.
        ].
        
        size := size - 1.
        theHashMask := IntNumber new(size).
        
        theHashTable := Array new(aHashLength).
        theList := ArrayList new(aCapacity).
        
        theDefault := aDefault.
    ]
    
    setAt(int aKey, object anObject)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        self getItem(aHashCode, aKey); value := anObject
    ]
        
    setAt(long aKey, object anObject)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        self getItem(aHashCode, aKey); value := anObject
    ]
        
    setAt(real aKey, object anObject)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        self getItem(aHashCode, aKey); value := anObject
    ]
        
    setAt(char aKey, object anObject)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        self getItem(aHashCode, aKey); value := anObject
    ]
        
    setAt(literal aKey, object anObject)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        self getItem(aHashCode, aKey); value := anObject
    ]
        
    setAt(wide aKey, object anObject)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        self getItem(aHashCode, aKey); value := anObject.
    ]
    
    /// Sets a dictionary item with the specified key
    setAt(object aKey, object anObject)
    [
        int aHashCode := aKey hash_code.
        
        self getItem(aHashCode, aKey); value := anObject.
    ]
        
    private stacksafe getItem(int aCode, object aKey)
    [
        int aHashCode := aCode && theHashMask.
        
        var aHashItem := theHashTable getAt(aHashCode).
        if (nil == aHashItem)
        [
            aHashItem := IntArray new(5).
            
            aHashItem write(0, 1).
            
            theHashTable setAt(aHashCode, aHashItem)
        ].
        
        auto aBacket := T<IntArray>(aHashItem).
        int  aLength := 0.
        aBacket read(0, &aLength).
        
        int i := 1.
        while (i < aLength)
        [
            int anItemIndex := 0.
            aBacket read(i, &anItemIndex).
            
            var anItem := theList getAt(anItemIndex).
            
            if(aKey equal(anItem key))
                [ ^ anItem ].
            
            i := i + 1
        ].
        
        var aNewItem := KeyValue new(aKey, theDefault).
        
        theList readLengthTo(&i).
        theList append:aNewItem.
        
        // Extend a backet
        int aCapacity := 0.
        aBacket readLengthTo(&aCapacity).
        if (aCapacity == aLength)
            [
                auto aNewBacket := IntArray new(aCapacity + 5).
                aNewBacket write(0, aCapacity, aBacket).

                aBacket := aNewBacket.
                theHashTable setAt(aHashCode, aNewBacket).
            ].
            
        aBacket write(aLength, i).
            
        aLength := aLength + 1.
        aBacket write(0, aLength).
        
        ^ aNewItem
    ]    
        
    getAt : aKey
    [
        int aHashCode := aKey hash_code.
        
        ^ self getItem(aHashCode, aKey); value.
    ]
            
    getAt(int aKey)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        ^ self getItem(aHashCode, aKey); value.
    ]
        
    getAt(long aKey)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        ^ self getItem(aHashCode, aKey); value.
    ]
        
    getAt(real aKey)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        ^ self getItem(aHashCode, aKey); value.
    ]
        
    getAt(char aKey)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        ^ self getItem(aHashCode, aKey); value.
    ]
        
    getAt(literal aKey)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        ^ self getItem(aHashCode, aKey); value.
    ]
        
    getAt(wide aKey)
    [
        int aHashCode := 0.
        aKey readHashCodeTo(&aHashCode).
        
        ^ self getItem(aHashCode, aKey); value.
    ]

    bool containsKey(object aKey)
    [
        int aHashCode := aKey hash_code.
        
        aHashCode := aHashCode && theHashMask.
        
        var aHashItem := theHashTable getAt(aHashCode).
        if (nil == aHashItem)
        [
            aHashItem := IntArray new(5).
            
            aHashItem write(0, 1).
            
            theHashTable setAt(aHashCode, aHashItem)
        ].
        
        auto aBacket := T<IntArray>(aHashItem).
        int  aLength := 0.
        aBacket read(0, &aLength).
        
        int i := 1.
        while (i < aLength)
        [
            int anItemIndex := 0.
            aBacket read(i, &anItemIndex).
            
            var anItem := theList getAt(anItemIndex).
            
            if(aKey equal(anItem key))
                [ ^ true ].
            
            i := i + 1
        ].        
        
        ^ false
    ]

    /// Returns a dictionary enumerator
    T<Enumerator> enumerator => theList.
    
    cast T<ArrayList>
        = theList clone.

    T<ArrayList> values = theList clone.

    /// Clones a dictionary
    clone 
    [
        var aList := Dictionary new.

        T<Enumerator> it := theList enumerator.
        while (it next)
        [
            var anItem := it get.
            
            aList[anItem key] := anItem value.
        ].
        
        ^ aList.
    ]
    
    clear
    [
        theHashTable := Array new(theHashTable length).
        theList clear.
    ]
}
