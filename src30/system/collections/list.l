#define system.
#define system'routines.

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
            
    #method Next = theNext.

    #method set &Next:aNext
    [
        theNext := aNext.
        
        ^ theNext.
    ]
}

// --- KeyValue ---

#class KeyValue
{
    #field theKey.
    #field theValue.
    
    #constructor new &key:aKey &value:anObject
    [
        theKey := aKey.
        theValue := anObject.
    ]
    
    #method key = theKey.
    
    #method value = theValue.
    
    #method $setValue : aValue
    [
        theValue := aValue.
    ]
}

// --- ListEnumerator ---

#class ListEnumerator
{
    #field theList.
    #field theCurrent.
    #field theState.
    
    #constructor new &list:aList
    [
        theList := aList.
        
        $self reset.
    ]

    #method reset
    [
        theState := %bof.
        theCurrent := nil.
    ]
            
    #method get = theCurrent get.
    
    #method set : aValue
    [
        theCurrent set:aValue.
    ]

    #method next => theState.

    #method next &bof
    [
        theCurrent := theList TopItem.
        theState := %active.
        
        ^ theCurrent != nil.
    ]
    
    #method next &active
    [
        theCurrent := theCurrent Next.
        
        ^ nil != theCurrent.
    ]
}

// --- List ---

#class(type:enumerable) List
{
    #field theTop.
    #field theLast.
    #field theState.
    #field theCount.
    
    #constructor new
    [
        theState := %empty.
        theCount := Integer new.
    ]

    #method TopItem = theTop.

    #method First = theTop get.

    #method length = theCount int.

    #method enumerable = $self.

    #method enumerator = ListEnumerator new &list:$self.

    #method append&empty : anObject
    [
        theCount << 1.
        theTop := ListItem new:anObject.
        theLast := theTop.
        theState := %active.
    ]

    #method append&active : anObject
    [
        theCount += 1.
        theLast := theLast set &Next:(ListItem new:anObject).
    ]

    #method getItem &index:anIndex
    [
        #var(type:int)aCurrentIndex := anIndex.
        #var anItem := theTop.
        
        #loop (aCurrentIndex > 0) ?
        [
            anItem := anItem Next.
            anItem == nil
                ? [ #throw OutOfRangeException new. ].
            
            aCurrentIndex := aCurrentIndex - 1.
        ].
        
        ^ anItem.
    ]

    #method getAt : anIndex
        = $self getItem &index:(anIndex int) get.

    #method setAt : anIndex : anObject
    [
        $self getItem &index::anIndex set:anObject.
    ]

    #method clear
    [
        theTop := nil.
        theLast := nil.
        theState := %empty.
        theCount << 0.        
    ]

    #method remove &index:anIndex
    [
        (theTop == nil)
            ? [ #throw OutOfRangeException new. ].
        
        (0 == anIndex)
            ? [
                theTop := theTop Next.
                theTop == nil
                    ? [ theLast := nil. theState := %empty. ].
            ]
            ! [
                #var aPrevious := $self getItem &index:(anIndex - 1).
                #var anItem := aPrevious Next.
                
                aPrevious set &Next:(anItem Next).
                
                anItem equal &reference:theLast
                    ? [ theLast := aPrevious ].
            ].
            
        theCount -= 1.
    ]
    
    #method append : anObject => theState.
}

#class Dictionary : List
{
    #field theDefaultValue.
    
    #constructor new &default:aValue
    [
        theState := %empty.
        theCount := Integer new.
        theDefaultValue := aValue.
    ]

    #method getItem &key:aKey
        = $self~enumerable seek: x [ x key == aKey ].
        
    #method append &key:aKey &value:aValue
        <= %append:(KeyValue new &key:aKey &value:aValue).

    #method getAt &key:aKey
    [
        #var anItem := $self getItem &key:aKey.
        ^ (nil == anItem)
            ? [ theDefaultValue ]
            ! [ anItem value ].
    ]

    #method set &key:aKey &value:aValue
    [
        #var anItem := $self getItem &key:aKey.
        (nil == anItem)
            ? [ $self append &key:aKey &value:aValue. ]
            ! [ anItem $setValue:aValue ].
    ]
}
