#define system.

#subject(class:system'collections'ListItem)   list_item.
#subject(class:system'collections'ListItem)   next_list_item.
#subject(class:system'collections'ListItem)   top_list_item.

#subject(class:system'collections'List)list.
#subject(class:system'collections'ArrayList)  array_list.
#subject(class:system'collections'Dictionary) dictionary.

#subject(class:system'IntNumber)              hash_length.

// --- ListItem ---

/// A list item
#class ListItem
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
    #method next_list_item = theNext.

    /// Sets the next item
    #method set &next_list_item:aNext
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
            ? [ theCurrent := theList top_list_item. ]
            ! [ theCurrent := theCurrent next_list_item. ].
            
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
        theCurrent := aList top_list_item.
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
    #method write &index:anIndex
    [
        #var(type:int)i := theIndex.
        
        (anIndex < i) ?
        [
            theCurrent := theList top_list_item.
            i := 0.
        ].
        
        #loop (i < anIndex) ?
        [
            ($nil == theCurrent)
                ? [ #throw OutOfRangeException new. ].
            
            theCurrent := theCurrent next_list_item.            
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
    #method top_list_item = theTop.

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
        
        theLast set &next_list_item:aNewItem.        
        theLast := aNewItem.
    ]

    #method insert&active : anObject
    [
        theCount += 1.        
        
        #var(type:list_item) aNewItem := ListItem new:anObject.
        aNewItem set &next_list_item:theTop.        
        theTop := aNewItem.
    ]
    
    /// Returns the object at the specified position
    #method getAt : anIndex
        = $self getAt &index:(anIndex int).

    /// Returns the list item at the specified position
    #method(type:list_item) getItemAt &index:anIndex
    [
        #var(type:int)aCurrentIndex := anIndex.
        #var(type:list_item) anItem := theTop.
        
        #loop (aCurrentIndex > 0) ?
        [
            anItem := anItem next_list_item.
            ($nil == anItem)
                ? [ #throw OutOfRangeException new. ].
            
            aCurrentIndex := aCurrentIndex - 1.
        ].
        
        ^ anItem.
    ]

    /// Returns the object at the specified position
    #method getAt &index:anIndex
        = $self getItemAt &index:anIndex get.

    /// Appends a new object to the end of the list
    #method append : anObject => theState.

    /// Inserts a new object to the beginning of the list
    #method insert : anObject => theState.

    /// Inserts a new object to the specified position
    #method insert &index:anIndex &object:anObject
    [
        (anIndex == 0)
            ? [ $self insert:anObject. ]
            ! [
                #var(type:list_item) aNew := ListItem new:anObject.        
                #var(type:list_item) aPrevious := $self getItemAt &index:(anIndex - 1).
                #var(type:list_item) aNext := aPrevious next_list_item.
                
                aPrevious set &next_list_item:aNew.
                aNew set &next_list_item:aNext.
            ].
    ]

    /// Removes an object at the specified position
    #method remove &index:anIndex
    [
        ($nil == theTop)
            ? [ #throw OutOfRangeException new. ].
        
        (0 == anIndex)
            ? [
                theTop := theTop next_list_item.
                ($nil == theTop)
                    ? [ theLast := $nil. theState := %empty. ].
            ]
            ! [
                #var(type:list_item) aPrevious := $self getItemAt &index:(anIndex - 1).
                #var(type:list_item) aNext := aPrevious next_list_item.
                
                aPrevious set &next_list_item:(aNext next_list_item).
                
                (aNext equal &reference:theLast)
                    ? [ theLast := aPrevious ].
            ].
            
        theCount -= 1.
    ]
        
    /// Returns the number of list items
    #method length = theCount int.
    
    /// Reads the number of list item to the output parameter
    #method readLength &vint:aLength
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
    
    /// Dispatches the verb to the target object with list subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &list:$self.
    
    /// Dispatches the verb to the target object with list subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &list:$self.

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
            
            anItem := anItem next_list_item.
        ].
    ]
}

// --- ArrayList ---

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
        theArray := Array new &length:32.
    ]
    
    /// Creates the list
    #constructor new &capacity:aCapacity
    [
        theCapacity := Integer new &int:aCapacity.
        theLength := Integer new.
        theArray := Array new &length:aCapacity.
    ]
    
    /// Creates the list
    #constructor $new &array:anArray &length:aLength
    [
        anArray readLength &vint:theCapacity.
        theLength := Integer new &int:aLength.
        
        theArray := anArray clone.
    ]
    
    /// Returns the object at the specified position
    #method getAt : anIndex
        = $self getAt &index:(anIndex int).

    /// Returns the object at the specified position
    #method getAt &index:anIndex
    [
        (theLength > anIndex)
            ! [ #throw OutOfRangeException new. ].

        ^ theArray getAt &index:anIndex.
    ]
    
    /// Returns the object at the specified position
    #method setAt : anIndex : anObject
        = $self setAt &index:(anIndex int) &object:anObject.

    /// Returns the object at the specified position
    #method setAt &index:anIndex &object:anObject
    [
        (theLength > anIndex)
            ! [ #throw OutOfRangeException new. ].
        
        theArray setAt &index:anIndex &object:anObject.
    ]
    
    #method $expand
    [
        (theCapacity <= theLength)?
        [
            theCapacity += 32.
            
            #var(type:array)aNewArray := Array new &length:theCapacity.
            #var(type:int) i := 0.
            #loop (i < theLength)?
            [
                aNewArray setAt &index:i &object:(theArray getAt &index:i).
                
                i := i + 1.
            ].
        ].
    ]
    
    /// Appends a new object to the end of the list
    #method append : anObject
    [
        $self $expand.
        
        theArray setAt &index:theLength &object:anObject.
        
        theLength += 1.
    ]
    
    /// Inserts a new object to the specified position
    #method insert &index:anIndex &object:anObject
    [
        (anIndex >= theLength)
            ? [ #throw OutOfRangeException new. ].
        
        $self $expand.
        
        #var(type:int) i := theLength.
        #loop (i > anIndex)?
        [
            theArray setAt &index:i &object:(theArray getAt &index:(i - 1)).
            
            i := i - 1.
        ].
        
        theArray setAt &index:anIndex &object:anObject.
    ]    
    
    /// Inserts a new object to the beginning of the list
    #method insert : anObject
        <= insert &index:0 &object:anObject.

    /// Removes an object at the specified position
    #method remove &index:anIndex
    [
        (anIndex >= theLength)
            ? [ #throw OutOfRangeException new. ].
        
        theLength -= 1.
        
        #var(type:int)i := anIndex.
        #loop (i < theLength)?
        [
            theArray setAt &index:i &object:(theArray getAt &index:(i + 1)).
        ].
    ]

    /// Returns itself
    #method array_list = $self.
    
    /// Dispatches the verb to the target object with array_list subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &array_list:$self.
    
    /// Dispatches the verb to the target object with array_list subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &array_list:$self.
    
    /// Returns the number of list items
    #method length = theLength int.
    
    /// Reads the number of list item to the output parameter
    #method readLength &vint:aLength
    [
        aLength << theLength.
    ]
    
    /// Clears the list
    #method clear 
    [
        theLength write &int:0.        
    ]
    
    /// Returns the top object
    #method first = theArray getAt &index:0.

    /// Returns the last object
    #method last = theArray getAt &index:(theLength - 1).
    
    /// Returns the list enumerator
    #method enumerator = ArrayListEnumerator new &array_list:$self.
    
    /// Returns the list iterator
    #method iterator = ArrayListIterator new &array_list:$self.
    
    /// Returns a list copy
    #method clone
        = ArrayList $new &array:theArray &length:theLength.
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
    #method get = theList getAt &index:theIndex.
    
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
    #method get = theList getAt &index:theIndex.
    
    /// Sets the current member
    #method set : anObject 
    [
        theList setAt &index:theIndex &object:anObject.
    ]
    
    /// Reads the current index into an output parameter
    #method readIndex &vint:anIndex
    [
        anIndex << theIndex.
    ]
    
    /// Sets the index by specified value
    #method write &index:anIndex
    [
        #var(type:int)aLength.
        theList readLength &vint:aLength.
        
        (aLength > anIndex)
            ! [ #throw OutOfRangeException new. ].
            
        theIndex << anIndex.
    ]
}

// --- Dictionary ---

#class(sealed) Dictionary
{
    #field(type:array_list) theList.
    #field(type:array)      theHashTable.
    #field(type:int)        theHashMask.

    #constructor new
        <= new &capacity:32 &hash_length:32.

    #constructor new &capacity:aCapacity &hash_length:aHashLength
    [
        #var(type:int)size := 32.
        #loop (size < aHashLength)?
        [
            size := size shift &index:-1.
        ].
        
        size := size - 1.
        theHashMask := IntNumber new &int:size.
        
        theHashTable := Array new &length:aHashLength.
        theList := ArrayList new &capacity:aCapacity.
    ]
    
    #method getAt : aKey
        = aKey cast:%getAt &to:$self.
        
    #method setAt : aKey : anObject
    [
        #var(type:int)aHashCode := aKey hash_code.
        
        $self $getItem &hash_code:aHashCode &key:aKey set &object:anObject.
    ]
        
    #method $getItem &hash_code:aCode &key:aKey
    [
        #var(type:int)aHashCode := aCode && theHashMask.
        
        #var aHashItem := theHashTable getAt &index:aHashCode.
        ($nil == aHashItem) ?
        [
            aHashItem := IntArray new &length:5.
            
            aHashItem write &index:0 &int:1.
            
            theHashTable setAt &index:aHashCode &object:aHashItem.
        ].
        
        #var(type:intarray) aBacket := aHashItem intarray.        
        #var(type:int)aLength := 0.        
        aBacket read &index:0 &vint:aLength.
        
        #var(type:int)i := 1.
        #loop (i < aLength)?
        [
            #var(type:int)anItemIndex.
            aBacket read &index:i &vint:anItemIndex.
            
            #var anItem := theList getAt &index:anItemIndex.
            
            (aKey == anItem key)
                ? [ ^ anItem. ].
            
            i := i + 1.
        ].
        
        #var aNewItem := KeyValue new &key:aKey &object:$nil.
        
        theList readLength &vint:i.
        theList append:aNewItem.
        
        // Extend a backet
        #var(type:int)aCapacity.
        aBacket readLength &vint:aCapacity.
        (aCapacity == aLength)
            ? [
                #var(type:intarray)aNewBacket := IntArray new &length:(aCapacity + 5).
                aNewBacket write &index:0 &length:aCapacity &intarray:aBacket.

                aBacket := aNewBacket.
            ].
            
        aBacket write &index:aLength &int:i.
            
        aLength := aLength + 1.
        aBacket write &index:0 &int:aLength.
        
        ^ aNewItem.
    ]    
        
    #method(generic)getAt : aKey
    [
        #var(type:int)aHashCode := aKey hash_code.
        
        ^ $self $getItem &hash_code:aHashCode &key:aKey object.
    ]
            
    #method getAt &int:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &hash_code:aHashCode &key:aKey object.
    ]
        
    #method getAt &long:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &hash_code:aHashCode &key:aKey object.
    ]
        
    #method getAt &real:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &hash_code:aHashCode &key:aKey object.
    ]
        
    #method getAt &char:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &hash_code:aHashCode &key:aKey object.
    ]
        
    #method getAt &literal:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &hash_code:aHashCode &key:aKey object.
    ]
        
    #method getAt &wide:aKey
    [
        #var(type:int)aHashCode.
        aKey readHashCode &vint:aHashCode.
        
        ^ $self $getItem &hash_code:aHashCode &key:aKey object.
    ]

    #method dictionary = $self.

    #method enumerator => theList.
    
    /// Dispatches the verb to the target object with dictionary subject
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &dictionary:$self.
    
    /// Dispatches the verb to the target object with dictionary subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &dictionary:$self.

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
