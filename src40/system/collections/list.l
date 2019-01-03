//// --- listof ---
//
//public sealed List<type> :: Enumerable<type>
//{
//    Array<type> theArray.
//    ref<int>    theLength.
//    ref<int>    theCapacity.
//
//    constructor(Array list)
//    [
//        int len := list length.
//        int size := len / 32.
//        size := size + 1.
//        size := size * 32.
//        
//        theLength := len.
//        theCapacity := size.
//        theArray := V<type>(size).
//        
//        int i := 0.
//        while(i < len)
//        [
//            theArray[i] := list[i].
//            
//            i := i + 1.
//        ].
//    ]
//
//    constructor(Array<type> list)
//    [
//        int len := list length.
//        int size := len / 32.
//        size := size + 1.
//        size := size * 32.
//        
//        theLength := len.
//        theCapacity := size.
//        theArray := V<type>(size).
//        
//        int i := 0.
//        while(i < len)
//        [
//            theArray[i] := list[i].
//            
//            i := i + 1.
//        ].
//    ]
//        
//    constructor(Array<type> list, int len)
//    [
//        int size := len / 32.
//        size := size + 1.
//        size := size * 32.
//        
//        theLength := len.
//        theCapacity := size.
//        theArray := V<type>(size).
//        
//        int i := 0.
//        while(i < len)
//        [
//            theArray[i] := list[i].
//            
//            i := i + 1.
//        ].
//    ]
//        
//    constructor(Enumerator<type> e)
//    [
//        theCapacity := 16.
//        theLength := 0.
//        theArray := V<type>(16).
//
//        while(e next)
//        [
//            self append(e get)
//        ]
//    ]
//    
//    constructor(int capacity)
//    [
//        theCapacity := capacity.
//        theLength := 0.
//        theArray := V<type>(capacity).
//    ]
//    
//    constructor
//    [
//        theCapacity := 16.
//        theLength := 0.
//        theArray := V<type>(16).
//    ]
//    
//    int rvprop length :: readLengthTo.
//
//    private expand
//    [
//        if (theCapacity <= theLength)
//        [
//            theCapacity append(32).
//            
//            Array<type> newArray := V<type>(theCapacity).
//
//            int i := 0.
//            while (i < theLength)
//            [
//                newArray setAt(i, theArray getAt(i)).
//                
//                i := i + 1.
//            ].
//            
//            theArray := newArray.
//        ].
//    ]
//    
//    /// Returns the object at the specified position
//    type getAt : anIndex
//        = self getAt(T<int>(anIndex)).
//
//    /// Returns the object at the specified position
//    type getAt(int index)
//    [
//        ifnot(index < theLength)
//            [ OutOfRangeException new; raise ].
//            
//        ^ theArray getAt(index)
//    ]
//
//    /// Returns the object at the specified position
//    setAt(int index, type object)
//    [
//        ifnot(index < theLength)
//            [ OutOfRangeException new; raise ].
//        
//        theArray setAt(index, object).
//    ]
//
//    /// Returns the object at the specified position
//    setAt(object anIndex, object anObject)
//        = self setAt(T<int>(anIndex), T<type>(anObject)).
//
//    /// Appends a new object to the end of the list
//    append(type object)
//    [
//        self expand.
//        
//        theArray setAt(T<int>(theLength), object).
//        
//        theLength append(1).
//    ]
//
//    append : object
//        <= append(T<type>(object)).
//
//    /// Removes an object at the specified position
//    remove(int anIndex)
//    [
//        ifnot (anIndex < theLength)
//            [ OutOfRangeException new; raise ].
//        
//        theLength reduce(1).
//        
//        int i := anIndex.
//        while(i < theLength)
//        [
//            theArray setAt(i, theArray getAt(i + 1)).
//            
//            i := i + 1
//        ]
//    ]
//        
//    /// Returns the top object
//    type first = theArray getAt(0).
//    
//    /// Clears the list
//    clear 
//    [
//        theLength value := 0.        
//    ]
//    
//    trim(IntNumber pos)
//    [
//        if (pos > theLength)
//            [ OutOfRangeException new; raise ].
//        
//        theLength value := pos.
//    ]
//    
//    Enumerator<type> enumerator = BaseEnumerator<type>::
//    {
//        @theIndex := -1.
//        
//        @theLength := self length.
//        
//        type get = self getAt(@theIndex).
//        
//        enumerable = self.
//        
//        reset
//        [
//            @theIndex := -1
//        ]
//    }.
//
//    Indexer<type> indexer = BaseIndexer<type>::
//    {
//        @theIndex := 0.
//        
//        @theLength := self length.
//        
//        type get
//            = self getAt(theIndex).
//            
//        set(type o)
//        [
//            self setAt(theIndex, o).
//        ]
//    }.    
//        
//    cast T<Array>
//    [
//        int len := T<int>(theLength).
//        auto copy := Array new(len).
//        int i := 0.
//        while (i < len)
//        [
//            copy[i] := theArray[i].
//            
//            i += 1.
//        ].
//        
//        ^ copy.
//    ]
//
//    Array<type> value
//    [
//        int len := T<int>(theLength).
//        Array<type> copy := V<type>(len).
//        
//        int i := 0.
//        while (i < len)
//        [
//            copy[i] := theArray[i].
//            
//            i += 1.
//        ].
//        
//        ^ copy.
//    ]
//
//    readLengthTo(ref<int> retVal)
//    [
//        theLength readValueTo(retVal).
//    ]    
//    
//    clone
//        = List<type>(theArray, T<int>(theLength)).
//}
//
//// --- ListItem ---
//
///// A list item
//sealed class ListItem
//{
//    object      theContent.
//    T<ListItem> theNext.
//    
//    /// Creates a new item
//    constructor new : aValue
//    [
//        theContent := aValue.
//    ]
//
//    /// Returns the item object
//    get = theContent.
//            
//    /// Sets the item object
//    set : anObject
//    [
//        theContent := anObject.
//    ]    
//
//    /// Returns the next item
//    T<ListItem> nextItem = theNext.
//
//    /// Sets the next item
//    setNextItem(ListItem aNext)
//    [
//        theNext := aNext.
//    ]
//}
//
//// --- ListEnumerator ---
//
///// A list enumerator
//class ListEnumerator :: Enumerator
//{
//    T<List>     theList.
//    T<ListItem> theCurrent.
//    
//    /// Creates a list enumerator
//    constructor new(List aList)
//    [
//        theList := aList.
//    ]
//
//    /// Creates a list enumerator
//    constructor new : anObject
//        <= new(T<List>(anObject)).
//
//    /// Returns the current object
//    get = theCurrent get.
//    
//    /// Resets the enumerator
//    reset
//    [
//        theCurrent := nil.
//    ]
//
//    /// Goes to the next enumeration member and returns true if it is not the last member
//    bool next
//    [
//        if (nil == theCurrent)
//            [ theCurrent := theList topItem ];
//            [ theCurrent := theCurrent nextItem ].
//            
//        ^ nil != theCurrent
//    ]
//    
//    enumerable = theList.
//}
//
//// --- ListIndexer ---
//
///// A list indexer
//class ListIndexer :: Indexer
//{
//    T<List>     theList.
//    T<ListItem> theCurrent.
//    ref<int>    theIndex.
//    
//    /// Creates a list indexer
//    constructor new(List aList)
//    [
//        theList := aList.
//        theCurrent := aList topItem.
//        theIndex := 0.
//    ]
//
//    /// Creates a list indexer
//    constructor new : anObject
//        <= new(T<List>(anObject)).
//    
//    /// Returns false if the index out of range
//    bool available = nil != theCurrent.
//
//    /// Returns the current member
//    get = theCurrent get.
//    
//    /// Sets the current member
//    set : anObject 
//    [
//        theCurrent set:anObject.
//    ]
//    
//    /// Reads the current index into an output parameter
//    readIndexTo(ref<int> anIndex)
//    [
//        theIndex readValueTo(anIndex).
//    ]
//    
//    /// Sets the index by specified value
//    writeIndex(int anIndex)
//    [
//        int i := theIndex.
//        
//        if(anIndex < i)
//        [
//            theCurrent := theList topItem.
//            i := 0.
//        ].
//        
//        while (i < anIndex) 
//        [
//            if (nil == theCurrent)
//                [ OutOfRangeException new; raise. ].
//            
//            theCurrent := theCurrent nextItem.            
//            i := i + 1
//        ].
//        
//        theIndex value := i.
//    ]
//    
//    appendIndex(int anIndex)
//    [
//        int i := theIndex.
//        i := i + anIndex.
//        
//        self writeIndex(i).
//    ]
//    
//    readLengthTo(ref<int> retVal)
//    [
//        theList readLengthTo(retVal).
//    ]
//}
//
//// --- List ---
//
///// A list object
//public sealed class List :: Enumerable
//{
//    ref<int>    theCount.
//    T<ListItem> theTop.
//    T<ListItem> theLast.
//
//    /// Creates a list
//    constructor new
//    [
//        theCount := 0.
//    ]
//
//    constructor new(Array l)
//    [
//        theCount := 0.
//        
//        auto it := l enumerator.
//        while (it next)
//        [
//            self append(it get)
//        ].
//    ]
//
//    /// Returns the top item
//    internal T<ListItem> topItem = theTop.
//
//    /// Returns the top object
//    first = theTop get.
//
//    /// Returns the last object
//    last = theLast get.    
//
//    append : anObject
//    [
//        theCount append(1).
//        if(nil == theTop)
//        [
//            theTop := ListItem new:anObject.
//            theLast := theTop.
//        ];
//        [
//            auto aNewItem := ListItem new:anObject.
//            
//            theLast setNextItem(aNewItem).
//            theLast := aNewItem.
//        ].
//
//    ]
//
//    insert : anObject
//    [
//        theCount append(1).
//        
//        auto aNewItem := ListItem new:anObject.
//        aNewItem setNextItem(theTop).
//        theTop := aNewItem.
//    ]
//    
//    /// Returns the list item at the specified position
//    internal T<ListItem> getItemAt(int anIndex)
//    [
//        int         aCurrentIndex := anIndex.
//        T<ListItem> anItem := theTop.
//        
//        while (aCurrentIndex > 0)
//        [
//            anItem := anItem nextItem.
//            if (nil == anItem)
//                [ OutOfRangeException new; raise ].
//            
//            aCurrentIndex := aCurrentIndex - 1
//        ].
//        
//        ^ anItem.
//    ]
//
//    /// Returns the object at the specified position
//    getAt(int anIndex)
//        = self getItemAt(anIndex); get.
//
//    /// Returns the object at the specified position
//    setAt(int anIndex, object anObject)
//        = self getItemAt(anIndex); set:anObject.
//
//    insert : anObject at:anIndex
//        <= insert(anIndex, anObject).
//
//    /// Inserts a new object to the specified position
//    insert(int anIndex, object anObject)
//    [
//        if (anIndex == 0)
//            [ self insert:anObject ];
//            [
//                T<ListItem> aNew := ListItem new:anObject.        
//                T<ListItem> aPrevious := self getItemAt(anIndex - 1).
//                T<ListItem> aNext := aPrevious nextItem.
//                
//                aPrevious setNextItem(aNew).
//                aNew setNextItem(aNext)
//            ]
//    ]
//
//    /// Removes an object at the specified position
//    remove(int anIndex)
//    [
//        if (nil == theTop)
//            [ OutOfRangeException new; raise ].
//        
//        if (0 == anIndex)
//            [
//                theTop := theTop nextItem.
//                if (nil == theTop)
//                    [ theLast := nil. ]
//            ];
//            [
//                T<ListItem> aPrevious := self getItemAt(anIndex - 1).
//                T<ListItem> aNext := aPrevious nextItem.
//                
//                aPrevious setNextItem(aNext nextItem).
//                
//                if(aNext equalReference:theLast)
//                    [ theLast := aPrevious ].
//            ].
//            
//        theCount reduce(1).
//    ]
//        
//    removeAt:anIndex
//        <= remove(T<int>(anIndex)).
//
//    int rvprop length :: readLengthTo.
//        
//    /// Reads the number of list item to the output parameter
//    readLengthTo(ref<int> aLength)
//    [
//        theCount readValueTo(aLength).
//    ]
//    
//    /// Clears the list
//    clear 
//    [
//        theCount value := 0.
//        
//        theTop := nil.
//        theLast := nil.
//    ]
//
//    /// Returns the list enumerator
//    T<Enumerator> enumerator = ListEnumerator new(self).
//
//    /// Returns the list indexer
//    T<Indexer> indexer = ListIndexer new(self).
//    
//    /// Clones the list
//    T<List> clone 
//    [
//        auto aList := List new.
//        
//        T<ListItem> anItem := theTop.        
//        while (nil != anItem)
//        [
//            aList append(anItem get).
//            
//            anItem := anItem nextItem
//        ].
//        
//        ^ aList
//    ]
//}

// --- ArrayList ---

/// An array list object
public sealed class ArrayList : Indexable//, interface<Enumerable>
{
    Array    theArray;
    ref<int> theLength;
    ref<int> theCapacity;

    /// Creates the list
    constructor()
    {
        theCapacity := 32;
        theLength := 0;
        theArray := Array.allocate(32);
    }

//    constructor new(int aCapacity)
//    [
//        theCapacity := aCapacity.
//        theLength := 0.
//        theArray := Array new(aCapacity).
//    ]
    
    constructor load(Array a)
        <= fromArray(a, a.Length);
    
    constructor load(o)
        <= ()
    {
        Enumerator e := o.enumerator();
        while (e.next())
        {
            self.append(e.get())
        }
    }
    
    /// Creates the list
    internal constructor fromArray(Array array, int length)
    {
        int capacity := array.Length;
        
        theCapacity := capacity;
        theLength := length;
        
        theArray := array.clone()
    }
    
    vprop int Length
    {
        /// Reads the number of list item to the output parameter
        read(ref int retVal)
        {
            theLength.readValueTo(ref retVal)
        }
    }

//    int rvprop capacity :: readCapacityTo.
    
    /// Returns the object at the specified position
    at(int index)
    {
        ifnot ((cast int(theLength)) > index)
            { OutOfRangeException.raise() };

        ^ theArray[index]
    }
    
    /// Returns the object at the specified position
    setAt(int index, object o)
    {
        ifnot (cast int(theLength) > index)
            { OutOfRangeException.raise() };
        
        theArray.setAt(index, o)
    }
    
    private expand()
    {
        if (theCapacity <= theLength)
        {
            theCapacity.append(32);
            
            Array newArray := Array.allocate(cast int(theCapacity));
            for(IntNumber i := 0, i < theLength, i := i + 1)
            {
                newArray[i] := theArray[i];
            };
            
            theArray := newArray
        }
    }
    
    /// Appends a new object to the end of the list
    append(item)
    {
        self.expand();
        
        theArray[cast int(theLength)] := item;
        
        theLength.append(1)
    }
    
//    appendRange : subarray
//    [
//        if (nil != subarray)
//        [
//            int len := subarray length.
//            int i := 0.
//            while (i < len)
//            [
//                self append(subarray getAt(i)).
//                i := i + 1
//            ]
//        ]
//    ]
//
//    appendRange(Indexable indexable)
//    [
//        if (nil != indexable)
//        [
//            int len := indexable length.
//            T<Indexer> it := indexable indexer.
//            
//            int i := 0.
//            while (it available)
//            [                
//                self append(it get).
//                
//                i += 1.
//                it writeIndex(i).
//            ]
//        ]
//    ]        
//                
//    /// Inserts a new object to the specified position
//    insert(int anIndex, object anObject)
//    [
//        if (anIndex >= theLength)
//            [ OutOfRangeException new; raise ].
//        
//        self expand.
//        
//        int i := theLength.
//        while (i > anIndex)
//        [
//            theArray setAt(i, theArray getAt(i - 1)).
//            
//            i := i - 1
//        ].
//        
//        theArray setAt(anIndex, anObject)
//    ]    
//    
//    /// Inserts a new object to the beginning of the list
//    insert : anObject
//        <= insert(0, anObject).
//
//    insert : anObject at:anIndex
//        <= insert(T<int>(anIndex), anObject).
        
    /// Removes an object at the specified position
    remove(int index)
    {
        if (index >= (cast int(theLength)))
            { OutOfRangeException.raise() };
        
        theLength.reduce(1);
        
        int i := index;
        while (i < (cast int(theLength)))
        {
            theArray.setAt(i, theArray.at(i + 1));
            
            i := i + 1
        }
    }
        
    removeAt(n)
        <= remove(cast int(n));
    
//    readCapacityTo(ref<int>{ aCapacity)
//    [
//        aCapacity value := theCapacity.
//    ]
//    
//    /// Clears the list
//    clear 
//    [
//        theLength value := 0
//    ]
//    
//    trim(int pos)
//    [
//        if (pos > theLength)
//            [ OutOfRangeException new; raise ].
//        
//        theLength value := pos.
//    ]
//    
//    /// Returns the top object
//    first = theArray getAt(0).
//
//    /// Returns the last object
//    last
//    [
//        int len := theLength.
//        
//        ^ theArray getAt(len - 1)
//    ]
    
    /// Returns the list enumerator
    Enumerator enumerator() = new ArrayListEnumerator(self);
    
    /// Returns the list indexer9
    Indexer indexer() = new ArrayListIndexer(self);
    
    /// Returns a list copy
    ArrayList clone()
        = ArrayList.fromArray(theArray, cast int(theLength));
        
    Array cast()
        = new Array(0, cast int(theLength), theArray);

//    value 
//        = Array new(0, T<int>(theLength), theArray).
}

// --- ArrayListEnumerator ---

/// A list enumerator
class ArrayListEnumerator : Enumerator
{
    ArrayList theList;
    ref<int>  theIndex;
    
    /// Creates a list enumerator
    constructor(ArrayList list)
    {
        theList := list;
        theIndex := -1;
    }

//    /// Creates a list enumerator
//    constructor new : anObject
//        <= new(T<ArrayList>(anObject)).

    /// Returns the current object
    get() = theList.at(cast int(theIndex));
    
    /// Resets the enumerator
    reset()
    {
        theIndex.Value := -1
    }

    /// Goes to the next enumeration member and returns true if it is not the last member
    bool next()
    {
        theIndex.append(1);
        
        int len := theList.Length;
        
        ^ len > (cast int(theIndex))
    }
    
    enumerable() = theList;
}

// --- ArrayListIndexer ---

/// A list indexer
public class ArrayListIndexer : Indexer
{
    ArrayList theList;
    ref<int>  theIndex;
    
    /// Creates a list indexer
    constructor(ArrayList list)
    {
        theList := list;
        theIndex := 0
    }

//    /// Creates a list indexer
//    constructor new : anObject
//        <= new array_list:anObject.
    
    /// Returns false if the index out of range
    get bool Available()
    {
        int length := theList.Length;
        
        ^ length > (cast int(theIndex))
    }

    /// Returns the current member
    get()
    {
        int index := theIndex.Value;
        
        ^ theList.at(index)
    }
    
    /// Sets the current member
    set(o)
    {
        int index := theIndex.Value;
        
        theList.setAt(index, o)
    }
    
    /// Reads the current index into an output parameter
    readIndexTo(ref int retVal)
    {
        theIndex.readValueTo(ref retVal)
    }
    
    appendIndex(int n)
    { 
        theIndex.append(n)
    } 
    
    /// Sets the index by specified value
    writeIndex(int n)
    {
        theIndex.Value := n
    }
    
//    readLengthTo(ref<int> retVal)
//    [
//        theList readLengthTo(retVal).
//    ]
}

// --- Dictionary ---

/// Dictionary class
public sealed  class Dictionary //: Enumerable
{
    ArrayList theList;
    Array     theHashTable;
    int       theHashMask;
    
    object    theDefault;

    /// Creates a dictionary
    constructor()
        <= new(32, 32, nil);

    constructor new(object default)
        <= new(32, 32, default);

    constructor new(int capacity, object default)
        <= new(capacity, 32, default);

    /// Creates a dictionary
    constructor new(int capacity, int hashLength, object default)
    {
        int size := 32;
        while (size < hashLength)
        {
            size := size $shl 1;
        };
        
        size := size - 1;
        theHashMask := new IntNumber(size);
        
        theHashTable := Array.allocate(hashLength);
        theList := ArrayList.allocate(capacity);
        
        theDefault := default;
    }
    
    setAt(int key, object object)
    {
        int hashCode := object.HashCode;
        
        self.getItem(hashCode, key).Value := object
    }
        
    setAt(long key, object object)
    {
        int hashCode := object.HashCode;
        
        self.getItem(hashCode, key).Value := object
    }
        
    setAt(real key, object object)
    {
        int hashCode := object.HashCode;
        
        self.getItem(hashCode, key).Value := object
    }
        
    setAt(char key, object object)
    {
        int hashCode := object.HashCode;
        
        self.getItem(hashCode, key).Value := object
    }
        
    setAt(string key, object object)
    {
        int hashCode := object.HashCode;
        
        self.getItem(hashCode, key).Value := object
    }
        
    setAt(wide key, object object)
    {
        int hashCode := object.HashCode;
        
        self.getItem(hashCode, key).Value := object
    }
    
    /// Sets a dictionary item with the specified key
    setAt(object key, object object)
    {
        int hashCode := object.HashCode;
        
        self.getItem(hashCode, key).Value := object
    }
        
//    private getItem(int code, object key)
//    {
//        int hashCode := code && theHashMask;
//        
//        var hashItem := theHashTable.at(hashCode);
//        if (nil == hashItem)
//        {
//            hashItem := new int[](5);
//            
//            hashItem[0] := 1;
//            
//            theHashTable.setAt(hashCode, hashItem)
//        };
//        
//        int[] backet := /*cast int[](*/hashItem/*)*/;
//        int  length := backet[0];
//        
//        for (int i := 1, i < length, i += 1)
//        {
//            int itemIndex := backet[i];
//            
//            var item := theList.at(itemIndex);
//            
//            if(key.safeEqual(item.key()))
//                { ^ item }
//        };
//        
//        var newItem := new KeyValue(key, theDefault);
//        
//        int i := theList.Length;
//        
//        theList.append:newItem;
//        
//        // Extend a backet
//        int capacity := backet.Length;
//        if (capacity == length)
//        {
//            auto newBacket := new int[](capacity + 5);
//            newBacket.write(0, capacity, backet);
//
//            backet := newBacket;
//            theHashTable.setAt(hashCode, newBacket)
//        };
//            
//        backet[length] := i;
//            
//        length := length + 1;
//        backet[0] := length;
//        
//        ^ newItem
//    }    
        
//    getAt : aKey
//    [
//        int aHashCode := aKey hash_code.
//        
//        ^ self getItem(aHashCode, aKey); value.
//    ]
//            
//    getAt(int aKey)
//    [
//        int aHashCode := 0.
//        aKey readHashCodeTo(&aHashCode).
//        
//        ^ self getItem(aHashCode, aKey); value.
//    ]
//        
//    getAt(long aKey)
//    [
//        int aHashCode := 0.
//        aKey readHashCodeTo(&aHashCode).
//        
//        ^ self getItem(aHashCode, aKey); value.
//    ]
//        
//    getAt(real aKey)
//    [
//        int aHashCode := 0.
//        aKey readHashCodeTo(&aHashCode).
//        
//        ^ self getItem(aHashCode, aKey); value.
//    ]
//        
//    getAt(char aKey)
//    [
//        int aHashCode := 0.
//        aKey readHashCodeTo(&aHashCode).
//        
//        ^ self getItem(aHashCode, aKey); value.
//    ]
//        
//    getAt(literal aKey)
//    [
//        int aHashCode := 0.
//        aKey readHashCodeTo(&aHashCode).
//        
//        ^ self getItem(aHashCode, aKey); value.
//    ]
//        
//    getAt(wide aKey)
//    [
//        int aHashCode := 0.
//        aKey readHashCodeTo(&aHashCode).
//        
//        ^ self getItem(aHashCode, aKey); value.
//    ]
//
//    bool containsKey(object aKey)
//    [
//        int aHashCode := aKey hash_code.
//        
//        aHashCode := aHashCode && theHashMask.
//        
//        var aHashItem := theHashTable getAt(aHashCode).
//        if (nil == aHashItem)
//        [
//            aHashItem := IntArray new(5).
//            
//            aHashItem write(0, 1).
//            
//            theHashTable setAt(aHashCode, aHashItem)
//        ].
//        
//        auto aBacket := T<IntArray>(aHashItem).
//        int  aLength := 0.
//        aBacket read(0, &aLength).
//        
//        int i := 1.
//        while (i < aLength)
//        [
//            int anItemIndex := 0.
//            aBacket read(i, &anItemIndex).
//            
//            var anItem := theList getAt(anItemIndex).
//            
//            if(aKey equal(anItem key))
//                [ ^ true ].
//            
//            i := i + 1
//        ].        
//        
//        ^ false
//    ]
//
//    /// Returns a dictionary enumerator
//    T<Enumerator> enumerator => theList.
//    
//    cast T<ArrayList>
//        = theList clone.
//
//    T<ArrayList> values = theList clone.
//
//    /// Clones a dictionary
//    clone 
//    [
//        var aList := Dictionary new.
//
//        T<Enumerator> it := theList enumerator.
//        while (it next)
//        [
//            var anItem := it get.
//            
//            aList[anItem key] := anItem value.
//        ].
//        
//        ^ aList.
//    ]
//    
//    clear
//    [
//        theHashTable := Array new(theHashTable length).
//        theList clear.
//    ]
}
