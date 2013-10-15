#define system.

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
    
    #constructor new &Key:aKey &Value:anObject
    [
        theKey := aKey.
        theValue := anObject.
    ]
    
    #method Key = theKey.
    
    #method Value = theValue.
    
    #method $setValue : aValue
    [
        theValue := aValue.
    ]
}

// --- ListEnumerator ---

#symbol EEnumStart =
{
    next
        => %$start.
}.

#symbol EEnumNext =
{
    next
        => %$next.
}.

#class ListEnumerator
{
    #field theList.
    #field theCurrent.
    #field theRole.
    
    #constructor new : aList
    [
        theList := aList.
        theRole := EEnumStart.
        theCurrent := nil.
    ]
    
    #method $start
    [
        theCurrent := theList Top.
        theRole := EEnumNext.
        
        ^ theCurrent != nil.
    ]    

    #method reset
    [
        theRole := EEnumStart.
        theCurrent := nil.
    ]
            
    #method get = theCurrent get.
    
    #method set : aValue
    [
        theCurrent set:aValue.
    ]
    
    #method $next
    [
        theCurrent := theCurrent Next.
        
        #var b := nil != theCurrent.
        
        ^ nil != theCurrent.
    ]
    
    #method => theRole.
}

// --- List ---

#symbol CEmptyList =
{
    Count = 0.
    
    append : anObject
        => %$addFirst.
        
    append &Key:aKey &Value:aValue
        => %$addFirst.
}.

#symbol CList =
{
    append : anObject
        => %$addNext.
        
    append &Key:aKey &Value:aValue
        => %$addNext.
}.

#class List
{
    #field theTop.
    #field theLast.
    #field theRole.
    #field theCount.
    
    #constructor new
    [
        theRole := CEmptyList.
        theCount := Integer new.
    ]

    #method Top = theTop.

    #method Count = theCount Number.

    #method Length = theCount Number.

    #method Enumerator = ListEnumerator new:$self.

    #method $addFirst : anObject
    [
        theCount << 1.
        theTop := ListItem new:anObject.
        theLast := theTop.
        theRole := CList.
    ]

    #method $addNext : anObject
    [
        theCount += 1.
        theLast := theLast set &Next:(ListItem new:anObject).
    ]

    #method <= &&type'enumerable:$self.
    
    #method => theRole.
}

#class Dictionary : List
{
    #method $addFirst &Key:aKey &Value:aValue
    [
        theCount << 1.
        theTop := ListItem new:(KeyValue new &Key:aKey &Value:aValue).
        theLast := theTop.
        theRole := CList.
    ]

    #method $addNext &Key:aKey &Value:aValue
    [
        theCount += 1.
        theLast := theLast set &Next:(ListItem new:(KeyValue new &Key:aKey &Value:aValue)).
    ]

    #method getAt:aKey
    [
        #var anItem := $self~EList seek: &&:x [ x Key == aKey ].
        ^ (nil == anItem)
            ? [ nil ]
            ! [ anItem Value ].
    ]

    #method setAt:aKey:aValue
    [
        #var anItem := $self~EList seek: &&:x [ x Key == aKey ].
        (nil == anItem)
            ? [
                $self append &Key:aKey &Value:aValue.
            ]
            ! [
                anItem $setValue:aValue.
            ].
    ]
}
