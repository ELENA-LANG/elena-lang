#define system.

#subject(class:system'collections'List)list.
#subject(class:system'collections'Dictionary)dictionary.

// --- ListItem ---

#class ListItem
{
    #field theContent.
    #field theNext.
    
    #constructor new : aValue
    [
        theContent := aValue.
    ]

    #method get = theContent.
            
    #method set : anObject
    [
        theContent := anObject.
    ]    
            
    #method nextItem = theNext.

    #method set &nextItem:aNext
    [
        theNext := aNext.
        
        ^ theNext.
    ]
}

// --- KeyValue ---

#class KeyValue
{
    #field theKey.
    #field theObject.
    
    #constructor new &key:aKey &object:anObject
    [
        theKey := aKey.
        theObject := anObject.
    ]
    
    #method key = theKey.
    
    #method $getObject = theObject.
    
    #method $setObject : anObject
    [
        theObject := anObject.
    ]
    
    #method => theObject.
}

// --- ListEnumerator ---

#class ListEnumerator :: Enumerator
{
    #field theList.
    #field theCurrent.
    
    #constructor new : aList
    [
        theList := aList.
    ]

    #method get = theCurrent get.
    
    #method reset
    [
        theCurrent := nil.
    ]

    #method next
    [
        (nil equal &reference:theCurrent)
            ? [ theCurrent := theList topItem. ]
            ! [ theCurrent := theCurrent nextItem. ].
            
        ^ nil equal:theCurrent not.
    ]
}

// --- ListIndexer ---

#class ListIterator :: Iterator
{
    #field              theList.
    #field              theCurrent.
    #field(type:intvar) theIndex.
    
    #constructor new : aList
    [
        theList := aList.
        theCurrent := aList topItem.
        theIndex := Integer new.
    ]
    
    #method available = nil != theCurrent.

    #method get = theCurrent get.
    
    #method set : anObject 
    [
        theCurrent set:anObject.
    ]
    
    #method read &out'index:anIndex
    [
        anIndex := theIndex int.
    ]
    
    #method write &index:anIndex
    [
        #var(type:int)i := theIndex int.
        
        (i > anIndex) ?
        [
            theCurrent := theList topItem.
            i := 0.
        ].
        
        #loop (i < anIndex) ?
        [
            (nil == theCurrent)
                ? [ #throw OutOfRangeException new. ].
            
            theCurrent := theCurrent nextItem.            
            i := i + 1.
        ].
        
        theIndex write &int:anIndex.
    ]
}

// --- BaseList ---

#class BaseList
{
    #field theState.
    #field(type:intvar)theCount.
    #field theTop.
    #field theLast.

    #constructor new
    [
        theState := %empty.
        theCount := Integer new.
    ]

    #method topItem = theTop.

    #method first = theTop get.

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
        theLast := theLast set &nextItem:(ListItem new:anObject).
    ]

    #method insert&active : anObject
    [
        theCount += 1.
        
        theTop := ListItem new:anObject set &nextItem:theTop.
    ]
    
    #method getAt : anIndex
        = $self getAt &index:(anIndex int).

    #method getItemAt &index:anIndex
    [
        #var(type:int)aCurrentIndex := anIndex.
        #var anItem := theTop.
        
        #loop (aCurrentIndex > 0) ?
        [
            anItem := anItem nextItem.
            (anItem == nil)
                ? [ #throw OutOfRangeException new. ].
            
            aCurrentIndex := aCurrentIndex - 1.
        ].
        
        ^ anItem.
    ]

    #method getAt &index:anIndex
        = $self getItemAt &index:anIndex get.

    #method append : anObject => theState.

    #method insert : anObject => theState.

    #method insert &index:anIndex &object:anObject
    [
        (anIndex == 0)
            ? [ $self insert:anObject. ]
            ! [
                #var aPrevious := $self getItemAt &index:(anIndex - 1).
                #var aNext := aPrevious Next.
                
                aPrevious set &nextItem:(ListItem new:anObject set &nextItem:aNext).                
            ].
    ]

    #method remove &index:anIndex
    [
        (theTop == nil)
            ? [ #throw OutOfRangeException new. ].
        
        (0 == anIndex)
            ? [
                theTop := theTop nextItem.
                (theTop == nil)
                    ? [ theLast := nil. theState := %empty. ].
            ]
            ! [
                #var aPrevious := $self getAt &index:(anIndex - 1).
                #var anItem := aPrevious Next.
                
                aPrevious set &nextItem:(anItem nextItem).
                
                (anItem equal &reference:theLast)
                    ? [ theLast := aPrevious ].
            ].
            
        theCount -= 1.
    ]
        
    #method length = theCount int.
    
    #method read &out'length:aLength
    [
        aLength := theCount int.
    ]
    
    #method clear 
    [
        theCount write &int:0.
        
        theState := %empty.
        theTop := nil.
        theLast := nil.
    ]
    
    #method enumerator = ListEnumerator new:$self.
    
    #method iterator = ListIterator new:$self.
    
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &enumerable:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &enumerable:$self.
}

// --- List ---

#class(sealed) List :: BaseList
{
    #method list = $self.
    
    #method clone 
    [
        #var aList := List new.
        
        #var anItem := theTop.        
        #loop (anItem != nil) ?
        [
            aList append:(anItem get).
            
            anItem := anItem nextItem.
        ].
    ]
}

// --- Dictionary ---

#class(sealed) Dictionary :: BaseList
{
    #field theDefaultValue.
    
    #constructor new
        <= (%new)
    [
    ]
    
    #constructor new &default:aValue
        <= (%new)
    [
        theDefaultValue := aValue.
    ]

    #method append &key:aKey &object:aValue
        <= %append:(KeyValue new &key:aKey &object:aValue).

    #method insert &key:aKey &object:aValue
        <= %insert:(KeyValue new &key:aKey &object:aValue).

    #method insert &index:anIndex &key:aKey &object:aValue
        <= %insert &index:anIndex &object:(KeyValue new &key:aKey &object:aValue).

    #method findItem &key:aKey
        = x [ x key safeEqual:aKey ] retrieve &enumerator:($self enumerator).

    #method getAt &key:aKey
    [
        #var anItem := $self findItem &key:aKey.
        ^ (nil == anItem)
            ? [ theDefaultValue ]
            ! [ anItem $getObject ].
    ]

    #method set &key:aKey &object:aValue
    [
        #var anItem := $self findItem &key:aKey.
        (nil == anItem)
            ? [ $self append &key:aKey &object:aValue. ]
            ! [ anItem $setObject:aValue ].
    ]

    #method dictionary = $self.
    
    #method clone 
    [
        #var aList := Dictionary new.
        
        #var anItem := theTop.        
        #loop (anItem != nil) ?
        [
            #var aKeyValue := anItem get.
            
            aList append &key:(aKeyValue key) &object:(aKeyValue $getObject).
            
            anItem := anItem nextItem.
        ].
    ]
}
