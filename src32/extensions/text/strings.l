import extensions.

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

    multi find : anObject
        <= find int:anObject.

    multi append : anObject
        <= append int:anObject.

    multi insert : anObject
        <= insert(anObject literal).

    multi delete : anObject
        <= delete int:anObject.

    find int:anIndex
    [
        theIndex := anIndex.
    ]

    find literal:aValue
    [
        theIndex := theValue indexOf:aValue at:theIndex.
        
        ^ (-1 != theIndex)
    ]
    
    find wide:aValue
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
    
    append literal:aValue 
    [
        theValue := theValue + aValue.
    ]
    
    append wide:aValue 
    [
        theValue := theValue + aValue.
    ]
    
    append int:aValue 
    [
        theIndex := theIndex + aValue.
    ]
    
    insert literal:aValue
    [
        theValue := theValue insert:aValue at:theIndex.
        
        theIndex := theIndex + aValue length.
    ]
    
    insert wide:aValue
    [
        theValue := theValue insert:aValue at:theIndex.
        
        theIndex := theIndex + aValue length.
    ]
    
    delete int:aLength
    [
        theValue := theValue delete:aLength at:theIndex.
    ]

    delete literal:l
        <= delete int(l length).

    delete wide:l
        <= delete int(l length).

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
            [ ^theValue Substring(theIndex - theStart) at:theStart ]
    ]
    
    literal => theValue.
    
    wide => theValue.
    
    length => theValue.
    
    index = theIndex int.
}

// --- StringWriter ---

sealed class StringWriter :: BaseVariable
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

    writeFormatted : format : o1
        = $self printFormatted(format, o1).

    writeFormatted : format : o1 : o2
        = $self printFormatted(format, o1, o2).

    writeFormatted : format : o1 : o2: o3
        = $self printFormatted(format, o1, o2, o3).

    writeFormatted : format : o1 : o2: o3: o4
        = $self printFormatted(format, o1, o2, o3, o4).

    writeFormatted : format : o1 : o2: o3: o4: o5
        = $self printFormatted(format, o1, o2, o3, o4, o5).

    writeFormatted : format : o1 : o2: o3: o4: o5: o6
        = $self printFormatted(format, o1, o2, o3, o4, o5, o6).

    writeFormatted : format : o1 : o2: o3: o4: o5: o6: o7
        = $self printFormatted(format, o1, o2, o3, o4, o5, o6, o7).

    writeFormatted : format : o1 : o2: o3: o4: o5: o6: o7: o8
        = $self printFormatted(format, o1, o2, o3, o4, o5, o6, o7, o8).

    writeFormatted : format : o1 : o2: o3: o4: o5: o6: o7: o8: o9
        = $self printFormatted(format, o1, o2, o3, o4, o5, o6, o7, o8, o9).

    writeFormatted object&:args
        = $self printFormatted object&:args.

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

    literal = theValue.

    wide => theValue.
    
    value = theValue.

    dispatch => theValue.
}
