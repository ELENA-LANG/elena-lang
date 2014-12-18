#define system. 
#define system'routines. 

#symbol(const)$TextPageSize = 128.

#type(class:system'text'TextBuffer) textbuffer.

// --- TextBuffer ---

#class(sealed,type:textbuffer) TextBuffer
{
    #field(type:shortarray) theBuffer.
    #field(type:intvar) theLength.

    #constructor new
    [
        theBuffer := ShortArray new &length:$TextPageSize.
        theLength := Integer new &int:0.
    ]

    #method clear
    [
        theLength write &int:0.
    ]

    #method read &out'length:aLength
    [
        theLength read &out'int:aLength.
    ]

    #method length
        = theLength int.

    #method set &length:aLength
    [
        theLength write &int:aLength.
    ]

    #method write : aLine
    [
        $self write &literal::aLine.
    ]
                                    
//    #method write &char:aChar
//    [
//        $self write &literal::aChar.
//    ]
    
    #method reserve &length:aLength
    [
        #var(type:int)anOldLength := theLength int.
        #var(type:int)aNewLength := aLength.
        
        aNewLength := aNewLength + anOldLength.
        
        #var(type:int)aBufferLength := theBuffer length.
        
        (aNewLength >= aBufferLength) ?
        [
            aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            
            #var(type:shortarray) aNewBuffer := ShortArray new &length:aNewLength.
            
            aNewBuffer write &index:0 &length:aBufferLength &shortarray::theBuffer.
            
            theBuffer := aNewBuffer.
        ].
    ]

    #method write &literal:aLiteral
    [
        #var(type:int)anOldLength := theLength int.
        #var(type:int)aLength := aLiteral length.
        
        (0 == aLength) !
        [
            $self reserve &length:aLength.
            
            theBuffer write &index:anOldLength &length:aLength &literal:aLiteral.
            
            theLength append &int:aLength.
        ].
    ]

    #method write &short:aChar
    [
        #var(type:int)anOldLength := theLength int.
        
        $self reserve &length:1.
        
        theBuffer write &index:anOldLength &short:aChar.
        
        theLength append &int:1.
    ]

    #method write &shortarray:anArray &length:aLength
    [
        #var(type:int)anOldLength.
        theLength read &out'int:anOldLength.
        
        (0 == aLength) !
        [
            $self reserve &length:aLength.
            
            theBuffer write &index:anOldLength &length:aLength &shortarray:anArray.
            
            theLength append &int:aLength.
        ].
    ]

//    #method indexOf &index:aPosition &char:aChar &out'int:aRetVal
//    [
//        #var(type:short)aValue.
//        aChar read &out'short:aValue.
//        
//        $self indexOf &index:aPosition &short:aValue &out'int:aRetVal.
//    ]

    #method indexOf &index:aPosition &short:aValue &out'int:aRetVal
    [
        #var(type:int)aLength := theLength int.
        
        theBuffer indexOf &index:aPosition &length:aLength &short:aValue &out'int:aRetVal.
    ]

    #method Substring &index:anIndex &length:aLength
        = (aLength == 0)
            ? [ emptyLiteralValue ]
            ! [
                #var aLiteral := LiteralValue new &length:aLength &index:anIndex &shortarray:theBuffer.

                ^ aLiteral.        
            ].

    #method delete &index:anIndex &length:aLength
    [
        (anIndex == 0)
            ? [
                #var aRightLiteral := $self~literalOp Substring &index:aLength.
                
                $self clear write &literal::aRightLiteral.
            ]
            ! [
                #var aLeftLiteral := $self Substring &index:0 &length:anIndex.
                #var aRightLiteral := $self~literalOp Substring &index:(anIndex + aLength).
                
                $self clear write &literal::aLeftLiteral write &literal::aRightLiteral.
            ].
    ]
    
//    #method shortarray = theBuffer shortarray.

    #method literal
    [
        #var(type:int)aLength := theLength int.
        
        ^ LiteralValue new &length:aLength &shortarray:theBuffer.
    ]
}
