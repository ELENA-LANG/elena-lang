#define system.

// --- Bookmark ---

#class Bookmark
{
    #field theLiteral.
    #field theIndex.
    #field theStart.
    
    #constructor new
    [
        theLiteral := emptyLiteralValue.
        theIndex := 0.
        theStart := 0.
    ]

    #constructor new &type'widestr:aText
    [
        theLiteral := aText.
        theIndex := 0.
        theStart := 0.
    ]

    #constructor new &type'widestr:aText &type'index:anIndex
    [
        theLiteral := aText.
        theIndex := anIndex.
        theStart := 0.
    ]

    #method find &type'index:anIndex
    [
        theIndex := anIndex.
    ]
            
    #method find &type'widestr:aValue
    [
        theIndex := theLiteral indexOf &type'index:theIndex &type'widestr::aValue.
        
        ^ (-1 != theIndex).
    ]
    
    #method mark
    [
        (-1 == theIndex)
            ? [
                $self clear.
            ]
            ! [
                theIndex := 1 + theIndex.
                theStart := theIndex.
            ].
        
    ]
    
    #method append :aValue
        <= append::aValue.
    
    #method append &type'widestr:aValue
    [
        theLiteral := theLiteral + aValue.
    ]
    
    #method insert &type'widestr:aValue
    [
        theLiteral := theLiteral insert &type'widestr:aValue &type'index:theIndex.
        
        theIndex := theIndex + aValue type'length.
    ]
    
    #method delete &type'length:aLength
    [
        theLiteral := theLiteral delete &type'index:theIndex &type'length:aLength.
    ]

    #method clear
    [
        theLiteral := emptyLiteralValue.
        theIndex := 0.
        theStart := 0.
    ]
            
    #method Marked
        = (-1 == theIndex)
            ? [ theLiteral Substring &type'index:theStart ]
            ! [ theLiteral Substring &type'index:theStart &type'length:(theIndex - theStart) ].
    
    #method Literal = theLiteral.
    
    #method Length => theLiteral.
    
    #method Index = theIndex.
}
