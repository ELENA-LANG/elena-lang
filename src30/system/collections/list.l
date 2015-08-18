#import system.

#subject(class:system'collections'ListItem)   list_item.

#subject(class:system'collections'List)list.
#subject(class:system'collections'ArrayList)  array_list.
#subject(class:system'collections'Dictionary) dictionary.

// --- ListItem ---

/// A list item
#class(sealed) ListItem
{
    #field                 theContent.
    #field(type:list_item) theNext.
    
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
    #method(type:list_item) nextItem = theNext.

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
    #field(type:list)     theList.
    #field(type:list_item)theCurrent.
    
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
    #field(type:list)      theList.
    #field(type:list_item) theCurrent.
    #field(type:vint)      theIndex.
    
    /// Creates a list iterator
    #constructor new &list:aList
    [
        theList := aList.
        theCurrent := aList topItem.
        theIndex := Integer new.
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
        anIndex << theIndex.
    ]
    
    /// Sets the index by specified value
    #method writeIndex &int:anIndex
    [
        #var(type:int)i := theIndex.
        
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
        
        theIndex write &int:i.
    ]
}

// --- List ---

/// A list object
#class(sealed) List
{
    #field theState.
    #field(type:vint)      theCount.
    #field(type:list_item) theTop.
    #field(type:list_item) theLast.

    /// Creates a list
    #constructor new
    [
        theState := %empty.
        theCount := Integer new.
    ]

    /// Returns the top item
    #method(type:list_item) topItem = theTop.

    /// Returns the top object
    #method first = theTop get.

    /// Returns the last object
    #method last = theLast get.    

    #method append&empty : anObject
    [
        theCount write &int:1.
        theTop := ListItem new:anObject.
        theLast := theTop.
        theState := %active.
    ]

    #method insert&empty : anObject
        <= %append &empty:anObject.

    #method append&active : anObject
    [
        theCount += 1.
        
        #var(type:list_item) aNewItem := ListItem new:anObject.
        
        theLast setNextItem &list_item:aNewItem.        
        theLast := aNewItem.
    ]

    #method insert&active : anObject
    [
        theCount += 1.        
        
        #var(type:list_item) aNewItem := ListItem new:anObject.
        aNewItem setNextItem &list_item:theTop.        
        theTop := aNewItem.
    ]
    
    /// Returns the object at the specified position
    #method getAt : anIndex
        = $self getAt &int:(anIndex int).

    /// Returns the list item at the specified position
    #method(type:list_item,stacksafe) getItemAt &int:anIndex
    [
        #var(type:int)aCurrentIndex := anIndex.
        #var(type:list_item) anItem := theTop.
        
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
                #var(type:list_item) aNew := ListItem new:anObject.        
                #var(type:list_item) aPrevious := $self getItemAt &int:(anIndex - 1).
                #var(type:list_item) aNext := aPrevious nextItem.
                
                aPrevious setNextItem &list_item:aNew.
                aNew setNextItem &list_item:aNext.
            ].
    ]

    /// Removes an object at the specified position
    #method remove &int:anIndex
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
                #var(type:list_item) aPrevious := $self getItemAt &int:(anIndex - 1).
                #var(type:list_item) aNext := aPrevious nextItem.
                
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
        
    /// Returns the number of list items
    #method length = theCount int.
    
    /// Reads the number of list item to the output parameter
    #method(stacksafe) readLength &vint:aLength
    [
        aLength << theCount.
    ]
    
    /// Clears the list
    #method clear 
    [
        theCount write &int:0.
        
        theState := %empty.
        theTop := $nil.
        theLast := $nil.
    ]
    
    /// Returns the list enumerator
    #method enumerator = ListEnumerator new &list:$self.
    
    /// Returns the list iterator
    #method iterator = ListIterator new &list:$self.
    
    /// Dispatches the verb to the target object with enumerable subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &enumerable:$self.
    
    /// Dispatches the verb to the target object with enumerable subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &enumerable:$self.

    /// Returns itself
    #method list = $self.
    
    /// Clones the list
    #method clone 
    [
        #var aList := List new.
        
        #var(type:list_item) anItem := theTop.        
        #loop ($nil != anItem) ?
        [
            aList append:(anItem get).
            
            anItem := anItem nextItem.
        ].
    ]
}

// --- ArrayList ---

/// An array list object
#class(sealed) ArrayList
{
    #field(type:array) theArray.
    #field(type:vint)  theLength.
    #field(type:vint)  theCapacity.
    
    /// Creates the list
    #constructor new
    [
        theCapacity := Integer new &int:32.
        theLength := Integer new.
        theArray := Array new &int:32.
    ]
    
    /// Creates the list
    #constructor new : aCapacity
        <= new &int:(aCapacity int).
    
    #constructor new &int:aCapacity
    [
        theCapacity := Integer new &int:aCapacity.
        theLength := Integer new.
        theArray := Array new &int:aCapacity.
    ]
    
    /// Creates the list
    #constructor(stacksafe) $new &array:anArray &int:aLength
    [
        theCapacity := Integer new.
        theLength := Integer new &int:aLength.
        
        anArray readLength &vint:theCapacity.
        
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
            
            #var(type:array)aNewArray := Array new &int:theCapacity.
            #var(type:int) i := 0.
            #loop (i < theLength)?
            [
                aNewArray setAt &int:i &object:(theArray getAt &int:i).
                
                i := i + 1.
            ].
            
            theArray := aNewArray.
        ].
    ]
    
    /// Appends a new object to the end of the list
    #method append : anObject
    [
        $self $expand.
        
        theArray setAt &int:theLength &object:anObject.
        
        theLength += 1.
    ]
    
    /// Inserts a new object to the specified position
    #method insert &int:anIndex &object:anObject
    [
        (anIndex >= theLength)
            ? [ #throw OutOfRangeException new. ].
        
        $self $expand.
        
        #var(type:int) i := theLength.
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
        
        #var(type:int)i := anIndex.
        #loop (i < theLength)?
        [
            theArray setAt &int:i &object:(theArray getAt &int:(i + 1)).
        ].
    ]
        
    #method remove &index:anIndex
        <= remove &int:(anIndex int).

    #method remove &at:anIndex
        <= remove &int:(anIndex int).

    /// Returns itself
    #method array_list = $self.
    
    /// Dispatches the verb to the target object with enumerable subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &enumerable:$self.
    
    /// Dispatches the verb to the target object with enumerable subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &enumerable:$self.
    
    /// Returns the number of list items
    #method length = theLength int.
    
    /// Reads the number of list item to the output parameter
    #method(stacksafe) readLength &vint:aLength
    [
        aLength << theLength.
    ]
    
    /// Clears the list
    #method clear 
    [
        theLength write &int:0.        
    ]
    
    /// Returns the top object
    #method first = theArray getAt &int:0.

    /// Returns the last object
    #method last = theArray getAt &int:(theLength - 1).
    
    /// Returns the list enumerator
    #method enumerator = ArrayListEnumerator new &array_list:$self.
    
    /// Returns the list iterator9
    #method iterator = ArrayListIterator new &array_list:$self.
    
    /// Returns a list copy
    #method(type:array_list) clone
        = ArrayList $new &array:theArray &int:theLength.
        
    #method array
        = Array new &int:0 &int:theLength &array:theArray.
}

// --- ArrayListEnumerator ---

/// A list enumerator
#class ArrayListEnumerator :: Enumerator
{
    #field(type:array_list) theList.
    #field(type:vint)       theIndex.
    
    /// Creates a list enumerator
    #constructor new &array_list:aList
    [
        theList := aList.
        theIndex := Integer new &int:-1.
    ]

    /// Creates a list enumerator
    #constructor new : anObject
        <= new &array_list:(anObject array_list).

    /// Returns the current object
    #method get = theList getAt &int:theIndex.
    
    /// Resets the enumerator
    #method reset
    [
        theIndex << -1.
    ]

    /// Goes to the next enumeration member and returns true if it is not the last member
    #method next
    [
        theIndex += 1.
        
        #var(type:int)aLength.
        theList readLength &vint:aLength.
        
        ^ (aLength > theIndex).
    ]
}

// --- ArrayListIndexer ---

/// A list iterator
#class ArrayListIterator :: Iterator
{
    #field(type:array_list)theList.
    #field(type:vint)      theIndex.
    
    /// Creates a list iterator
    #constructor new &array_list:aList
    [
        theList := aList.
        theIndex := Integer new.
    ]

    /// Creates a list iterator
    #constructor new : anObject
        <= new &array_list:(anObject array_list).
    
    /// Returns false if the index out of range
    #method available
    [
        #var(type:int)aLength.
        theList readLength &vint:aLength.
        
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
        anIndex << theIndex.
    ]
    
    /// Sets the index by specified value
    #method writeIndex &int:anIndex
    [
        theIndex << anIndex.
    ]
}

// --- Dictionary ---

/// Dictionary class
#class(sealed) Dictionary
{
    #field(type:array_list) theList.
    #field(type:array)      theHashTable.
    #field(type:int)        theHashMask.
    
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
        #var(type:int)size := 32.
        #loop (size < aHashLength)?
        [
            size := size shift &int:-1.
        ].
        
        size := size - 1.
        theHashMask := IntNumber new &int:size.
        
        theHashTable := Array new &int:aHashLength.
        theList := ArrayList new &int:aCapacity.
        
        theDefault := aDefault.
    ]
    
    /// Gets a dictionary item with the specified key
    #method getAt : aKey
        = aKey cast:%getAt &to:$self.
        
    /// Sets a dictionary item with the specified key
    #method setAt : aKey : anObject
    [
        #var(type:int)aHashCode := aKey hash_code.
        
        $self $getItem &int:aHashCode &key:aKey set &object:anObject.
    ]
        
    #method(stacksafe) $getItem &int:aCode &key:aKey
    [
        #var(type:int)aHashCode := aCode && theHashMask.
        
        #var aHashItem := theHashTable getAt &int:aHashCode.
        ($nil == aHashItem) ?
        [
            aHashItem := IntArray new &int:5.
            
            aHashItem write &int:0 &int:1.
            
            theHashTable setAt &int:aHashCode &object:aHashItem.
        ].
        
        #var(type:intarray) aBacket := aHashItem intarray.        
        #var(type:int)aLength := 0.        
        aBacket read &int:0 &vint:aLength.
        
        #var(type:int)i := 1.
        #loop (i < aLength)?
        [
            #var(type:int)anItemIndex.
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
        #var(type:int)aCapacity.
        aBacket readLength &vint:aCapacity.
        (aCapacity == aLength)
            ? [
                #var(type:intarray)aNewBacket := IntArray new &int:(aCapacity + 5).
                aNewBacket write &int:0 &int:aCapacity &intarray:aBacket.

                aBacket := aNewBacket.
                theHashTable setAt &int:aHashCode &object:aNewBacket.
            ].
            
        aBacket write &int:aLength &int:i.
            
        aLength := aLength + 1.
        aBacket write &int:0 &int:aLength.
        
        ^ aNewItem.
    ]    
        
    #method(generic)getAt : aKey
    [
        #var(type:int)aHashCode := aKey hash_code.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
            
    #method getAt &int:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &long:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &real:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &char:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &literal:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]
        
    #method getAt &wide:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &int:aHashCode &key:aKey object.
    ]

    /// Returns itself
    #method dictionary = $self.

    /// Returns a dictionary enumerator
    #method enumerator => theList.
    
    #method array_list
        = theList clone.
            
    /// Dispatches the verb to the target object with enumerable subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &enumerable:$self.
    
    /// Dispatches the verb to the target object with enumerable subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &enumerable:$self.

    /// Clones a dictionary
    #method clone 
    [
        #var aList := Dictionary new.

        #var(type:enumerator) it := theList enumerator.
        #loop (it next)?
        [
            #var anItem := it get.
            
            aList@(anItem key) := anItem object.
        ].
        
        ^ aList.
    ]
}
