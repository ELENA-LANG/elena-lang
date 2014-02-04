#define system. 
#define system'routines. 
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
        theFreeLength := theBuffer size / 2.
        theBuffer clear.
    ]

    #method length
        = theBuffer length / 2.

    #method set &length:aLength
    [
        theBuffer set &length:(2 * aLength).
    ]
                                    
    #method write : aLine
    [
        $self write::aLine.
    ]
                                    
    #method write &char:aChar
    [
        $self write &literal::aChar.
    ]
    
    #method write &literal:aLiteral
    [
        #var(type:int)aLength := aLiteral length.

        (0 == aLength) !
        [
            (theFreeLength < aLength) ? 
            [
                #var(type:int) aSizeOffset := (1 + aLength / $TextPageSize) * $TextPageSize.

                theFreeLength := theFreeLength add &int:aSizeOffset.
                
                aSizeOffset := aSizeOffset * 2.
                
                theBuffer append &size:aSizeOffset.
            ].
            
            theBuffer~memoryOp write &literal:aLiteral.

            theFreeLength := theFreeLength subtract &int:aLength.
        ].
    ]

    #method indexOf &index:aPosition &char:aChar
    [
        #var(type:int) anIndex.
        theBuffer indexOf &index:(2 * aPosition) &short:aChar &out'int:anIndex.
        
        (-1 == anIndex) 
            ! [ anIndex := anIndex / 2. ].
            
        ^ anIndex.
    ]

    #method Substring &index:anIndex &length:aLength
        = (aLength == 0)
            ? [ emptyLiteralValue ]
            ! [
                #var aLiteral := LiteralValue new &length:aLength.
                
                theBuffer bytearray read &index:(anIndex * 2) &length:(aLength * 2) &bytearray:aLiteral.
                
                primitive refresh &literal:aLiteral.
                
                ^ aLiteral.        
            ].

    #method delete &index:anIndex &length:aLength
    [
        (anIndex == 0)
            ? [
                #var aRightLiteral := $self~literalOp Substring &index:aLength.
                
                $self clear write &literal:aRightLiteral.
            ]
            ! [
                #var aLeftLiteral := $self Substring &index:0 &length:anIndex.
                #var aRightLiteral := $self~literalOp Substring &index:(anIndex + aLength).
                
                $self clear write &literal:aLeftLiteral write &literal:aRightLiteral.                
            ].
    ]

    #method literal
        = theBuffer~memoryOp read &literal &length:(theBuffer length / 2).
}
