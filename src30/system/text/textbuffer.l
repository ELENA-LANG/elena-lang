#define system. 
#define system'routines. 

#symbol(const,type:int)$TextPageSize = 128.

#subject(class:system'text'TextBuffer) textbuffer.

// --- TextBuffer ---

#class(sealed) TextBuffer
{
    #field(type:chararray) theBuffer.
    #field(type:intvar) theLength.

    #constructor new
    [
        theBuffer := CharArray new &length:$TextPageSize.
        theLength := Integer new &int:0.
    ]

    #method clear
    [
        theLength write &int:0.
    ]

    #method read &out'length:aLength
    [
        aLength := theLength int.
    ]

    #method length
        = theLength int.

    #method set &length:aLength
    [
        theLength write &int:aLength.
    ]

    #method write : aLine
    [
        $self write &literal:(aLine literal).
    ]

    #method append : aLine
    [
        $self write &literal:(aLine literal).
    ]
                                    
    #method reserve &length:aLength
    [
        #var(type:int)anOldLength := theLength int.
        #var(type:int)aNewLength := aLength.
        
        aNewLength := aNewLength + anOldLength.
        
        #var(type:int)aBufferLength := theBuffer length.
        
        (aNewLength >= aBufferLength) ?
        [
            aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            
            #var(type:chararray) aNewBuffer := CharArray new &length:aNewLength.
            
            aNewBuffer write &index:0 &length:aBufferLength &chararray:theBuffer.
            
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

    #method write &char:aChar
    [
        #var(type:int)anOldLength := theLength int.
        
        $self reserve &length:1.
        
        theBuffer write &index:anOldLength &char:aChar.
        
        theLength append &int:1.
    ]

    #method write &chararray:anArray &length:aLength
    [
        #var(type:int)anOldLength := theLength int.
        
        (0 == aLength) !
        [
            $self reserve &length:aLength.
            
            theBuffer write &index:anOldLength &length:aLength &chararray:anArray.
            
            theLength append &int:aLength.
        ].
    ]

    #method insert &index:anIndex &literal:aLiteral
    [
        (anIndex == 0)
            ? [
                #var(type:literal) aRightLiteral := $self literal.
                
                $self clear write &literal:aLiteral write &literal:aRightLiteral.
            ]
            ! [
                #var(type:literal) aLeftLiteral := $self Substring &index:0 &length:anIndex literal.
                #var(type:literal) aRightLiteral := $self::literalOp Substring &index:anIndex.
                
                $self clear write &literal:aLeftLiteral write:aLiteral write &literal:aRightLiteral.
            ].
    ]
    
    #method insert &index:anIndex &char:aChar
        <= %insert &index:anIndex &literal:(aChar literal).

    #method indexOf &index:aPosition &char:aValue &out'int:aRetVal
    [
        #var(type:int)aLength := theLength int.
        
        aRetVal := theBuffer indexOf &index:aPosition &length:aLength &char:aValue.
    ]

    #method(type:literal) Substring &index:anIndex &length:aLength
    [
        (aLength == 0)
            ? [ ^ emptyLiteralValue. ].
            
        ^ LiteralValue new &length:aLength &index:anIndex &chararray:theBuffer.
    ]

    #method delete &index:anIndex &length:aLength
    [
        (anIndex == 0)
            ? [
                #var(type:literal) aRightLiteral := $self::literalOp Substring &index:aLength.
                
                $self clear write &literal:aRightLiteral.
            ]
            ! [
                #var(type:literal) aLeftLiteral := $self Substring &index:0 &length:anIndex.
                #var(type:literal) aRightLiteral := $self::literalOp Substring &index:(anIndex + aLength).
                
                $self clear write &literal:aLeftLiteral write &literal:aRightLiteral.
            ].
    ]
    
    #method chararray = theBuffer chararray.

    #method literal
    [
        #var(type:int)aLength := theLength int.
        
        ^ LiteralValue new &length:aLength &chararray:theBuffer.
    ]
}
