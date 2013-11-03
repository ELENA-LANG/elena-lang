#define system. 
#define system'io. 

#symbol $TextPageSize = 128.

// --- TextBuffer ---

#class TextBuffer
{
    #field theBuffer.
    #field theFreeLength.
    
    #constructor new
    [
        theBuffer := MemoryStream new &size:(2 * $TextPageSize).
        theFreeLength := $TextPageSize.
    ]

    #method clear
    [
        theFreeLength := theBuffer Size / 2.
        theBuffer clear.
    ]

    #method type'length
        = theBuffer type'length / 2.

    #method set &type'length:aLength
    [
        theBuffer set &type'length:(2 * aLength).
    ]
                                    
    #method append : aLine
    [
        $self append::aLine.
    ]
    
    #method append &type'widechar:aChar
    [
        $self append &type'widestr:(aChar Literal).
    ]
    
    #method append &type'widestr:aLiteral
    [
        #var aLength := aLiteral Length.

        (0 == aLength) !
        [
            (theFreeLength < aLength) ? 
            [
                #var aSizeOffset := (1 + aLength / $TextPageSize) * $TextPageSize.

                theBuffer expand &type'int:(2 * aSizeOffset).

                theFreeLength := theFreeLength + aSizeOffset.
            ].
            
            theBuffer~EStreamWriter write &type'widestr:aLiteral.

            theFreeLength := theFreeLength - aLength.
        ].

    ]

    #method indexOf &type'index:aPosition &type'widechar:aChar
    [
        #var anIndex := theBuffer indexOf &type'index:(2 * aPosition) &type'short:aChar.
        
        (-1 == anIndex) 
            ! [ anIndex := anIndex / 2. ].
            
        ^ anIndex.
    ]

    #method read &type'index:anIndex
        <= read &type'index:anIndex &type'length:($self Length - anIndex).

    #method read &type'index:anIndex &type'length:aLength
        = (aLength == 0)
            ? [ emptyLiteralValue ]
            ! [
                #var aLiteral := LiteralValue new &type'length:aLength.
                
                theBuffer type'bytearray read &type'index:(anIndex * 2) &type'length:(aLength * 2) &type'bytearray:aLiteral.
                
                system'external'CoreAPI refresh &type'widestr:aLiteral.
                
                ^ aLiteral.        
            ].

    #method delete &type'index:anIndex &type'length:aLength
    [
        (anIndex == 0)
            ? [
                #var aRightLiteral := $self read &type'index:aLength.
                
                $self clear append &type'widestr:aRightLiteral.
            ]
            ! [
                #var aLeftLiteral := $self read &type'index:0 &type'length:anIndex.
                #var aRightLiteral := $self read &type'index:(anIndex + aLength).
                
                $self clear append &type'widestr:aLeftLiteral append &type'widestr:aRightLiteral.                
            ].
    ]

    #method Literal
        = theBuffer~EStreamReader read &type'widestr &type'length:(theBuffer Length / 2).
}