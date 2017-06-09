// --- StringBookmark ---

class StringBookmark :: BaseValue
{
    object theValue.
    object theIndex.
    object theStart.
    
    constructor new
    [
        theValue := emptyLiteralValue.
        theIndex := 0.
        theStart := 0.
    ]

    constructor new : aText
    [
        theValue := aText.
        theIndex := 0.
        theStart := 0.
    ]

    equal int:aValue
        => theIndex.

    equal literal:aValue
        => theValue.

    equal wide:aValue
        => theValue.

    find : anObject
        = anObject cast:%eval to:
        {
            eval:aBookmark int : anIndex = aBookmark find index:anIndex.
            
            eval:aBookmark literal : aLiteral = aBookmark find value:aLiteral.
            
            eval:aBookmark wide : aLiteral = aBookmark find value:aLiteral.
        } with:self.

    append : anObject
        = anObject cast:%eval to:
        {
            eval:aBookmark int : anIndex = aBookmark append index:anIndex.
            
            eval:aBookmark literal : aLiteral = aBookmark append value:aLiteral.
            
            eval:aBookmark wide : aLiteral = aBookmark append value:aLiteral.
            
        } with:self.

    insert : anObject
        = anObject cast:%eval to:
        {
            eval:aBookmark literal : aLiteral = aBookmark insert value:aLiteral.
            
            eval:aBookmark wide : aLiteral = aBookmark insert value:aLiteral.
            
        } with:self.

    delete : anObject
        = anObject cast:%eval to:
        {
            eval:aBookmark int : aValue = aBookmark delete length:aValue.
            
            eval:aBookmark literal : aLiteral = aBookmark delete length:(aLiteral length).
            
            eval:aBookmark wide : aLiteral = aBookmark delete length:(aLiteral length).
            
        } with:self.

    find index:anIndex
    [
        theIndex := anIndex.
    ]

    find value:aValue
    [
        theIndex := theValue indexOf:aValue at:theIndex.
        
        ^ (-1 != theIndex)
    ]
    
    mark
    [
        if(-1 == theIndex)
            [
                $self clear
            ];
            [
                theIndex := 1 + theIndex.
                theStart := theIndex
            ].
        
    ]
    
    append value:aValue 
    [
        theValue := theValue + aValue.
    ]
    
    append index:aValue 
    [
        theIndex := theIndex + aValue.
    ]
    
    insert value:aValue
    [
        theValue := theValue insert:aValue at:theIndex.
        
        theIndex := theIndex + aValue length.
    ]
    
    delete length:aLength
    [
        theValue := theValue delete:aLength at:theIndex.
    ]

    clear
    [
        theValue := emptyLiteralValue.
        theIndex := 0.
        theStart := 0.
    ]

    Marked
    [
        if(-1 == theIndex)
            [ ^theValue Substring:theStart ];
            [ ^theValue Substring:(theIndex - theStart) at:theStart ]
    ]
    
    literal => theValue.
    
    wide => theValue.
    
    length => theValue.
    
    index = theIndex int.
}

// --- StringWriter ---

class StringWriter
{    
    literal theValue.
    
    constructor new
    [
        theValue := emptyLiteralValue.
    ]
    
    constructor new : aText
    [
        theValue := aText literal.
    ]
    
    write : aText
    [
        theValue := theValue add:aText.
    ]
    
    writeLine : aText
    [
        $self write:aText; write:'newLine.
    ]
   
    writeLine
    [
        $self write:'newLine.
    ]    

    set : aText
    [
        theValue := aText.
    ]

    append : aText
    [
        theValue := theValue add:aText.
    ]
    
    insert:s at:index
    [
        theValue := theValue insert:s at:index.
    ]
    
    get = theValue.

    clear
    [
        theValue := emptyLiteralValue.
    ]

    wide => theValue.

    /// Dispatches the verb to the target object
    cast : aVerb to:aTarget => theValue.
    
    /// Dispatches the verb to the target object
    cast : aVerb to:aTarget with:aParam => theValue.

    dispatch => theValue.
}
