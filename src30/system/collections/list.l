#import system.

#subject list_item  = system'collections'ListItem.
#subject list       = system'collections'List.
#subject array_list = system'collections'ArrayList.
#subject dictionary = system'collections'Dictionary.

//// --- listof ---
//
//#template listof : type
//{
//    #field(arrayof:type)theArray.
//    #field(vint)        theLength.
//    #field(vint)        theCapacity.
//
//    #method $expand
//    [
//        (theCapacity <= theLength)?
//        [
//            theCapacity += 32.
//            
//            #var(arrayof:type)newArray := type:theCapacity.
//            
//            #var(int) i := 0.
//            #loop (i < theLength)?
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
//    #method getAt : anIndex
//        = $self getAt &int:(anIndex int).
//
//    /// Returns the object at the specified position
//    #method(type,stacksafe)getAt &int:index
//    [
//        (index < theLength)
//            ! [ #throw OutOfRangeException new. ].
//            
//        ^ theArray getAt &int:index.
//    ]
//
//    /// Returns the object at the specified position
//    #method(stacksafe) setAt &int:index &object:object
//    [
//        (index < theLength)
//            ! [ #throw OutOfRangeException new. ].
//        
//        theArray setAt &int:index &object:object.
//    ]
//
//    /// Returns the object at the specified position
//    #method setAt : anIndex : anObject
//        = $self setAt &int:(anIndex int) &object:anObject.
//
//    #method append : object
//        <= append &type:object.
//
//    /// Appends a new object to the end of the list
//    #method append &type:object
//    [
//        $self $expand.
//        
//        theArray setAt &int:theLength &object:object.
//        
//        theLength += 1.
//    ]
//
//    /// Removes an object at the specified position
//    #method(stacksafe) remove &int:anIndex
//    [
//        (anIndex < theLength)
//            ! [ #throw OutOfRangeException new. ].
//        
//        theLength -= 1.
//        
//        #var(int)i := anIndex.
//        #loop (i < theLength)?
//        [
//            theArray setAt &int:i &type:(theArray getAt &int:(i + 1)).
//            
//            i += 1.
//        ].
//    ]
//        
//    /// Clears the list
//    #method clear 
//    [
//        theLength set &int:0.        
//    ]
//    
//    #method(w3) enumerator 
//    [
//        #var(vint)index := -1.
//        #var(int)length := theLength.
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
//    #method(outnprop:length,stacksafe)readLength &vint:retVal
//    [
//        theArray readLength &vint:retVal.
//    ]    
//}
//
//// --- ObjectList ---
//
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
#class(sealed) ListItem
{
    #field            theContent.
    #field(list_item) theNext.
    
    /// Creates a new item
    #constructor new : aValue
    [
        theContent := aValue.
    ]

    /// Returns the item object
    #method get = theContent.
            
    /// Sets the item object
    #method set : anObject
    [
        theContent := anObject.
    ]    

    /// Returns the next item
    #method(list_item) nextItem = theNext.

    /// Sets the next item
    #method setNextItem &list_item:aNext
    [
        theNext := aNext.
    ]
}

// --- ListEnumerator ---

/// A list enumerator
#class ListEnumerator :: Enumerator
{
    #field(list)     theList.
    #field(list_item)theCurrent.
    
    /// Creates a list enumerator
    #constructor new &list:aList
    [
        theList := aList.
    ]

    /// Creates a list enumerator
    #constructor new : anObject
        <= new &list:(anObject list).

    /// Returns the current object
    #method get = theCurrent get.
    
    /// Resets the enumerator
    #method reset
    [
        theCurrent := $nil.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        ($nil == theCurrent)
            ? [ theCurrent := theList topItem. ]
            ! [ theCurrent := theCurrent nextItem. ].
            
        ^ $nil != theCurrent.
    ]
}

// --- ListIndexer ---

/// A list iterator
#class ListIterator :: Iterator
{
    #field(list)      theList.
    #field(list_item) theCurrent.
    #field(vint)      theIndex.
    
    /// Creates a list iterator
    #constructor(w3) new &list:aList
    [
        theList := aList.
        theCurrent := aList topItem.
        theIndex := 0.
    ]

    /// Creates a list iterator
    #constructor new : anObject
        <= new &list:(anObject list).
    
    /// Returns false if the index out of range
    #method available = $nil != theCurrent.

    /// Returns the current member
    #method get = theCurrent get.
    
    /// Sets the current member
    #method set : anObject 
    [
        theCurrent set:anObject.
    ]
    
    /// Reads the current index into an output parameter
    #method readIndex &vint:anIndex
    [
        anIndex := theIndex.
    ]
    
    /// Sets the index by specified value
    #method writeIndex &int:anIndex
    [
        #var(int)i := theIndex.
        
        (anIndex < i) ?
        [
            theCurrent := theList topItem.
            i := 0.
        ].
        
        #loop (i < anIndex) ?
        [
            ($nil == theCurrent)
                ? [ #throw OutOfRangeException new. ].
            
            theCurrent := theCurrent nextItem.            
            i := i + 1.
        ].
        
        theIndex set &int:i.
    ]
}

// --- List ---

/// A list object
#class(sealed,dispatchable:enumerable) List
{
    #field theState.
    #field(vint)      theCount.
    #field(list_item) theTop.
    #field(list_item) theLast.

    /// Creates a list
    #constructor(w3) new
    [
        theState := %empty.
        theCount := 0.
    ]

    /// Returns the top item
    #method(list_item) topItem = theTop.

    /// Returns the top object
    #method first = theTop get.

    /// Returns the last object
    #method last = theLast get.    

    #method append&empty : anObject
    [
        theCount set &int:1.
        theTop := ListItem new:anObject.
        theLast := theTop.
        theState := %active.
    ]

    #method insert&empty : anObject
        <= %append &empty:anObject.

    #method append&active : anObject
    [
        theCount += 1.
        
        #var(list_item) aNewItem := ListItem new:anObject.
        
        theLast setNextItem &list_item:aNewItem.        
        theLast := aNewItem.
    ]

    #method insert&active : anObject
    [
        theCount += 1.        
        
        #var(list_item) aNewItem := ListItem new:anObject.
        aNewItem setNextItem &list_item:theTop.        
        theTop := aNewItem.
    ]
    
    /// Returns the object at the specified position
    #method getAt : anIndex
        = $self getAt &int:(anIndex int).

    /// Returns the object at the specified position
    #method setAt : anIndex : anObject
        = $self setAt &int:(anIndex int) &object:anObject.

    /// Returns the list item at the specified position
    #method(list_item,stacksafe) getItemAt &int:anIndex
    [
        #var(int)aCurrentIndex := anIndex.
        #var(list_item) anItem := theTop.
        
        #loop (aCurrentIndex > 0) ?
        [
            anItem := anItem nextItem.
            ($nil == anItem)
                ? [ #throw OutOfRangeException new. ].
            
            aCurrentIndex := aCurrentIndex - 1.
        ].
        
        ^ anItem.
    ]

    /// Returns the object at the specified position
    #method(stacksafe) getAt &int:anIndex
        = $self getItemAt &int:anIndex get.

    /// Returns the object at the specified position
    #method(stacksafe) setAt &int:anIndex &object:anObject
        = $self getItemAt &int:anIndex set:anObject.

    /// Appends a new object to the end of the list
    #method append : anObject => theState.

    /// Inserts a new object to the beginning of the list
    #method insert : anObject => theState.

    #method insert : anObject &at:anIndex
        <= insert &int:(anIndex int) &object:anObject.

    /// Inserts a new object to the specified position
    #method insert &int:anIndex &object:anObject
    [
        (anIndex == 0)
            ? [ $self insert:anObject. ]
            ! [
                #var(list_item) aNew := ListItem new:anObject.        
                #var(list_item) aPrevious := $self getItemAt &int:(anIndex - 1).
                #var(list_item) aNext := aPrevious nextItem.
                
                aPrevious setNextItem &list_item:aNew.
                aNew setNextItem &list_item:aNext.
            ].
    ]

    /// Removes an object at the specified position
    #method(w2) remove &int:anIndex
    [
        ($nil == theTop)
            ? [ #throw OutOfRangeException new. ].
        
        (0 == anIndex)
            ? [
                theTop := theTop nextItem.
                ($nil == theTop)
                    ? [ theLast := $nil. theState := %empty. ].
            ]
            ! [
                #var(list_item) aPrevious := $self getItemAt &int:(anIndex - 1).
                #var(list_item) aNext := aPrevious nextItem.
                
                aPrevious setNextItem &list_item:(aNext nextItem).
                
                (aNext equal &reference:theLast)
                    ? [ theLast := aPrevious ].
            ].
            
        theCount -= 1.
    ]
        
    #method remove &at:anIndex
        <= remove &int:(anIndex int).

    /// obsolete        
    #method remove &index:anIndex
        <= remove &int:(anIndex int).
        
    /// Reads the number of list item to the output parameter
    #method(stacksafe,outnprop:length) readLength &vint:aLength
    [
        aLength := theCount.
    ]
    
    /// Clears the list
    #method clear 
    [
        theCount set &int:0.
        
        theState := %empty.
        theTop := $nil.
        theLast := $nil.
    ]

    /// Returns the list enumerator
    #method enumerator = ListEnumerator new &list:$self.

    /// Returns the list iterator
    #method iterator = ListIterator new &list:$self.
    
    /// Clones the list
    #method clone 
    [
        #var aList := List new.
        
        #var(list_item) anItem := theTop.        
        #loop ($nil != anItem) ?
        [
            aList append:(anItem get).
            
            anItem := anItem nextItem.
        ].
    ]
}

// --- ArrayList ---

/// An array list object
#class(sealed,dispatchable:enumerable) ArrayList
{
    #field(array) theArray.
    #field(vint)  theLength.
    #field(vint)  theCapacity.
    
    /// Creates the list
    #constructor(w3) new
    [
        theCapacity := 32.
        theLength := 0.
        theArray := Array new &int:32.
    ]
    
    /// Creates the list
    #constructor(w2) new : param
        <= (new &int:(param cast:%eval &to:
                        { 
                            int : n = n. 
                            
                            array : a = a length.
                            
                            ! : n = 32. 
                        }))
    [
        $self append &array:(param cast:%eval &to:{ array : a = a. ! : o = $nil. }).
    ]
    
    #constructor(w3) new &int:aCapacity
    [
        theCapacity := aCapacity.
        theLength := 0.
        theArray := Array new &int:aCapacity.
    ]
    
    /// Creates the list
    #constructor(stacksafe,w3) $new &array:anArray &int:aLength
    [
        #var(int)capacity := anArray length.
        
        theCapacity := capacity.
        theLength := aLength.
        
        theArray := anArray clone.
    ]
    
    /// Returns the object at the specified position
    #method getAt : anIndex
        = $self getAt &int:(anIndex int).

    /// Returns the object at the specified position
    #method(stacksafe) getAt &int:anIndex
    [
        (theLength > anIndex)
            ! [ #throw OutOfRangeException new. ].

        ^ theArray getAt &int:anIndex.
    ]
    
    /// Returns the object at the specified position
    #method setAt : anIndex : anObject
        = $self setAt &int:(anIndex int) &object:anObject.

    /// Returns the object at the specified position
    #method(stacksafe) setAt &int:anIndex &object:anObject
    [
        (theLength > anIndex)
            ! [ #throw OutOfRangeException new. ].
        
        theArray setAt &int:anIndex &object:anObject.
    ]
    
    #method $expand
    [
        (theCapacity <= theLength)?
        [
            theCapacity += 32.
            
            #var(array)aNewArray := Array new &int:theCapacity.
            #var(int) i := 0.
            #loop (i < theLength)?
            [
                aNewArray setAt &int:i &object:(theArray getAt &int:i).
                
                i := i + 1.
            ].
            
            theArray := aNewArray.
        ].
    ]
    
    #method append &array:subarray
    [
        ($nil != subarray) ?
        [
            #var(int)len := subarray length.
            #var(int)i := 0.
            #loop (i < len)?
            [
                $self append &object:(subarray getAt &int:i).
                i := i + 1.
            ].
        ].        
    ]
        
    /// Appends a new object to the end of the list
    #method append &object:anObject
    [
        $self $expand.
        
        theArray setAt &int:theLength &object:anObject.
        
        theLength += 1.
    ]
    
    #method append : object
        <= append &object:object.
    
    /// Inserts a new object to the specified position
    #method insert &int:anIndex &object:anObject
    [
        (anIndex >= theLength)
            ? [ #throw OutOfRangeException new. ].
        
        $self $expand.
        
        #var(int) i := theLength.
        #loop (i > anIndex)?
        [
            theArray setAt &int:i &object:(theArray getAt &int:(i - 1)).
            
            i := i - 1.
        ].
        
        theArray setAt &int:anIndex &object:anObject.
    ]    
    
    /// Inserts a new object to the beginning of the list
    #method insert : anObject
        <= insert &int:0 &object:anObject.

    #method insert : anObject &at:anIndex
        <= insert &int:(anIndex int) &object:anObject.
        
    /// Removes an object at the specified position
    #method(stacksafe) remove &int:anIndex
    [
        (anIndex >= theLength)
            ? [ #throw OutOfRangeException new. ].
        
        theLength -= 1.
        
        #var(int)i := anIndex.
        #loop (i < theLength)?
        [
            theArray setAt &int:i &object:(theArray getAt &int:(i + 1)).
            
            i := i + 1.
        ].
    ]
        
    #method remove &index:anIndex
        <= remove &int:(anIndex int).

    #method remove &at:anIndex
        <= remove &int:(anIndex int).

    /// Returns itself
    #method array_list = $self.
    
    /// Reads the number of list item to the output parameter
    #method(stacksafe,outnprop:length) readLength &vint:aLength
    [
        aLength := theLength.
    ]
    
    #method(stacksafe,outnprop:capacity) readCapacity &vint:aCapacity
    [
        aCapacity := theCapacity.
    ]
    
    /// Clears the list
    #method clear 
    [
        theLength set &int:0.        
    ]
    
    /// Returns the top object
    #method first = theArray getAt &int:0.

    /// Returns the last object
    #method last
    [
        #var(int)len := theLength.
        
        ^ theArray getAt &int:(len - 1).
    ]
    
    /// Returns the list enumerator
    #method enumerator = ArrayListEnumerator new &array_list:$self.
    
    /// Returns the list iterator9
    #method iterator = ArrayListIterator new &array_list:$self.
    
    /// Returns a list copy
    #method(array_list) clone
        = ArrayList $new &array:theArray &int:theLength.
        
    #method array
        = Array new &int:0 &int:theLength &array:theArray.
        
    #method get
        <= array.
}

// --- ArrayListEnumerator ---

/// A list enumerator
#class ArrayListEnumerator :: Enumerator
{
    #field(array_list) theList.
    #field(vint)       theIndex.
    
    /// Creates a list enumerator
    #constructor(w3) new &array_list:aList
    [
        theList := aList.
        theIndex := -1.
    ]

    /// Creates a list enumerator
    #constructor new : anObject
        <= new &array_list:(anObject array_list).

    /// Returns the current object
    #method get = theList getAt &int:theIndex.
    
    /// Resets the enumerator
    #method reset
    [
        theIndex set &int:-1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        theIndex += 1.
        
        #var(int)aLength := theList length.
        
        ^ (aLength > theIndex).
    ]
}

// --- ArrayListIndexer ---

/// A list iterator
#class ArrayListIterator :: Iterator
{
    #field(array_list)theList.
    #field(vint)      theIndex.
    
    /// Creates a list iterator
    #constructor(w3) new &array_list:aList
    [
        theList := aList.
        theIndex := 0.
    ]

    /// Creates a list iterator
    #constructor new : anObject
        <= new &array_list:(anObject array_list).
    
    /// Returns false if the index out of range
    #method available
    [
        #var(int)aLength := theList length.
        
        ^ (aLength > theIndex).
    ]

    /// Returns the current member
    #method get = theList getAt &int:theIndex.
    
    /// Sets the current member
    #method set : anObject 
    [
        theList setAt &int:theIndex &object:anObject.
    ]
    
    /// Reads the current index into an output parameter
    #method readIndex &vint:anIndex
    [
        anIndex := theIndex.
    ]
    
    /// Sets the index by specified value
    #method writeIndex &int:anIndex
    [
        theIndex set &int:anIndex.
    ]
}

// --- Dictionary ---

/// Dictionary class
#class(sealed,dispatchable:enumerable) Dictionary
{
    #field(array_list) theList.
    #field(array)      theHashTable.
    #field(int)        theHashMask.
    
    #field theDefault.

    /// Creates a dictionary
    #constructor new
        <= new &int:32 &int:32 &object:$nil.

    #constructor new &default:aDefault
        <= new &int:32 &int:32 &object:aDefault.

    #constructor new:aCapacity &default:aDefault
        <= new &int:(aCapacity int) &int:32 &object:aDefault.

    /// Creates a dictionary
    #constructor new &int:aCapacity &int:aHashLength &object:aDefault
    [
        #var(int)size := 32.
        #loop (size < aHashLength)?
        [
            size := size << 1.
        ].
        
        size -= 1.
        theHashMask := IntNumber new &int:size.
        
        theHashTable := Array new &int:aHashLength.
        theList := ArrayList new &int:aCapacity.
        
        theDefault := aDefault.
    ]
    
    /// Gets a dictionary item with the specified key
    #method getAt : aKey
        = aKey cast:%getAt &to:$self.
        
    /// Sets a dictionary item with the specified key
    #method(w2) setAt : aKey : anObject
    [
        #var(int)aHashCode := aKey hash_code.
        
        $self $getItem &int:aHashCode &key:aKey set &object:anObject.
    ]
        
    #method(stacksafe,w2) $getItem &int:aCode &key:aKey
    [
        #var(int)aHashCode := aCode && theHashMask.
        
        #var aHashItem := theHashTable getAt &int:aHashCode.
        ($nil == aHashItem) ?
        [
            aHashItem := IntArray new &int:5.
            
            aHashItem write &int:0 &int:1.
            
            theHashTable setAt &int:aHashCode &object:aHashItem.
        ].
        
        #var(intarray) aBacket := aHashItem intarray.        
        #var(int)aLength := 0.
        aBacket read &int:0 &vint:aLength.
        
        #var(int)i := 1.
        #loop (i < aLength)?
        [
            #var(int)anItemIndex.
            aBacket read &int:i &vint:anItemIndex.
            
            #var anItem := theList getAt &int:anItemIndex.
            
            (aKey safeEqual:(anItem key))
                ? [ ^ anItem. ].
            
            i := i + 1.
        ].
        
        #var aNewItem := KeyValue new &key:aKey &object:theDefault.
        
        theList readLength &vint:i.
        theList append:aNewItem.
        
        // Extend a backet
        #var(int)aCapacity.
        aBacket readLength &vint:aCapacity.
        (aCapacity == aLength)
            ? [
                #var(intarray)aNewBacket := IntArray new &int:(aCapacity + 5).
                aNewBacket write &int:0 &int:aCapacity &intarray:aBacket.

                aBacket := aNewBacket.
                theHashTable setAt &int:aHashCode &object:aNewBacket.
            ].
            
        aBacket write &int:aLength &int:i.
            
        aLength := aLength + 1.
        aBacket write &int:0 &int:aLength.
        
        ^ aNewItem.
    ]    
        
    #method(generic,w2)getAt : aKey
    [
        #var(int)aHashCode := aKey hash_code.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
            
    #method getAt &int:aKey
    [
        #var(int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &long:aKey
    [
        #var(int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &real:aKey
    [
        #var(int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &char:aKey
    [
        #var(int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &literal:aKey
    [
        #var(int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &wide:aKey
    [
        #var(int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]

    /// Returns a dictionary enumerator
    #method enumerator => theList.
    
    #method array_list
        = theList clone.

    /// Clones a dictionary
    #method clone 
    [
        #var aList := Dictionary new.

        #var(enumerator) it := theList enumerator.
        #loop (it next)?
        [
            #var anItem := it get.
            
            aList@(anItem key) := anItem object.
        ].
        
        ^ aList.
    ]
    
    #method clear
    [
        theHashTable := Array new &int:(theHashTable length).
        theList clear.
    ]
}
