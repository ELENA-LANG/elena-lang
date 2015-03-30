#define system. 
#define system'routines. 

#symbol(const,type:int)$TextPageSize = 128.

#subject(class:system'text'TextBuffer) textbuffer.

// --- TextBuffer ---

#class(sealed) TextBuffer
{
    #field(type:chararray) theBuffer.
    #field(type:vint)      theLength.

    #constructor new
    [
        theBuffer := CharArray new &length:$TextPageSize.
        theLength := Integer new &int:0.
    ]

    #method clear
    [
        theLength write &int:0.
    ]

    #method readLength &vint:aLength
    [
        aLength << theLength.
    ]

    #method length
        = theLength int.

    #method set &length:aLength
    [
        theLength write &int:aLength.
    ]

    #method chararray = theBuffer.

    #method literal
        = LiteralValue new &length:theLength &index:0 &chararray:theBuffer.

    #method wide
        = WideLiteralValue new &length:theLength &index:0 &chararray:theBuffer.

    #method write : aLine
    [
        aLine cast:%write &to:$self.
    ]

    #method writeLine : aLine
    [
        $self write:aLine write:'newLine.
    ]

    #method append : aLine
    [
        $self write:aLine.
    ]
                                    
    #method reserve &length:aLength
    [
        #var(type:int)aNewLength := aLength.
        
        aNewLength := aNewLength + theLength.
        
        #var(type:vint)aBufferLength.
        theBuffer readLength &vint:aBufferLength.
        
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
        #var(type:vint)aLength.
        aLiteral readLength &vint:aLength.
        
        (0 == aLength) !
        [
            $self reserve &length:aLength.
            
            aLiteral save &index:theLength &chararray:theBuffer &vlength:aLength.
            
            theLength += aLength.
        ].
    ]

    #method write &wide:aLiteral
    [
        #var(type:vint)aLength.
        aLiteral readLength &vint:aLength.
        
        (0 == aLength) !
        [
            $self reserve &length:aLength.
            
            aLiteral save &index:theLength &chararray:theBuffer &vlength:aLength.
            
            theLength += aLength.
        ].
    ]

    #method write &char:aChar
    [
        $self reserve &length:1.
        
        theBuffer write &index:theLength &char:aChar.
        
        theLength += 1.
    ]

    #method(generic) write : anObject
    [
        $self write &literal:(anObject literal).
    ]

    #method write &chararray:anArray &length:aLength
    [
        (0 == aLength) !
        [
            $self reserve &length:aLength.
            
            theBuffer write &index:theLength &length:aLength &chararray:anArray.
            
            theLength += aLength.
        ].
    ]

    #method insert &index:anIndex &length:aLength &chararray:anArray
    [
        $self reserve &length:aLength.
        
        theBuffer move &index:anIndex &length:theLength &offset:aLength.
        
        theBuffer write &index:anIndex &length:aLength &chararray:anArray.
        
        theLength += aLength.
    ]

    #method insert &index:anIndex &literal:aLiteral
    [
        #var(type:vint)aLength.
        UTF8Encoding charCount &literal:aLiteral &vint:aLength.

        $self reserve &length:aLength.
        
        theBuffer move &index:anIndex &length:theLength &offset:aLength.
        
        aLiteral save &index:anIndex &chararray:theBuffer &vlength:aLength.
        
        theLength += aLength.
    ]

    #method insert &index:anIndex &wide:aLiteral
    [
        #var(type:vint)aLength.
        UTF16Encoding charCount &wide:aLiteral &vint:aLength.

        $self reserve &length:aLength.
        
        theBuffer move &index:anIndex &length:theLength &offset:aLength.
        
        aLiteral save &index:anIndex &chararray:theBuffer &vlength:aLength.
        
        theLength += aLength.
    ]
    
    #method insert &index:anIndex &char:aChar
    [
        $self reserve &length:1.
        
        theBuffer move &index:anIndex &length:theLength &offset:1.
        
        theBuffer write &index:anIndex &char:aChar.
        
        theLength += 1.
    ]

    #method indexOf &index:aPosition &char:aValue &vint:aRetVal
    [
        #var(type:int)anIndex := 0.
        #var(type:vchar)aCurrent.
        
        #loop (anIndex < theLength)?
        [
            theBuffer read &index:anIndex &vchar:aCurrent.
            
            (aCurrent == aValue)
                ? [ aRetVal << anIndex. ^$self. ].
                
            anIndex := anIndex + 1.
        ].
        
        aRetVal << -1.
    ]

    #method indexOf &index:aPosition &literal:aLiteral &vint:aRetVal
    [
        #var(type:vint)aLiteralLength.
        #var(type:int)anIndex := 0.
        #var(type:vchar)aCurrent.
        #var(type:vchar)aFirstChar.
        #var(type:vchar)aNextChar.
        
        aLiteral readLength &vint:aLiteralLength.
        
        aLiteral read &index:0 &vchar:aFirstChar.
        
        #loop (anIndex < theLength)?
        [
            theBuffer read &index:anIndex &vchar:aCurrent.
            
            (aCurrent == aFirstChar) ?
            [ 
                #var(type:int)j := 1.
                #loop (j < aLiteralLength) ?
                [
                    theBuffer read &index:(anIndex + j) &vchar:aCurrent.
                    aLiteral read &index:j &vchar:aNextChar.
                    
                    (aNextChar != aCurrent)
                        ? [ j := aLiteralLength. ].
                    
                    j := j + 1.
                ].
                
                (j > aLiteralLength)
                    ? [ aRetVal << anIndex. ^ $self. ].
            ].
                
            anIndex := anIndex + 1.
        ].
        
        aRetVal << -1.
    ]

    #method indexOf &index:aPosition &wide:aLiteral &vint:aRetVal
    [
        #var(type:vint)aLiteralLength.
        #var(type:int)anIndex := 0.
        #var(type:vchar)aCurrent.
        #var(type:vchar)aFirstChar.
        #var(type:vchar)aNextChar.
        
        aLiteral readLength &vint:aLiteralLength.
        
        aLiteral read &index:0 &vchar:aFirstChar.
        
        #loop (anIndex < theLength)?
        [
            theBuffer read &index:anIndex &vchar:aCurrent.
            
            (aCurrent == aFirstChar) ?
            [ 
                #var(type:int)j := 1.
                #loop (j < aLiteralLength) ?
                [
                    theBuffer read &index:(anIndex + j) &vchar:aCurrent.
                    aLiteral read &index:j &vchar:aNextChar.
                    
                    (aNextChar != aCurrent)
                        ? [ j := aLiteralLength. ].
                    
                    j := j + 1.
                ].
                
                (j > aLiteralLength)
                    ? [ aRetVal << anIndex. ^ $self. ].
            ].
                
            anIndex := anIndex + 1.
        ].
        
        aRetVal << -1.
    ]

    #method indexOf &index:aPosition &char:aValue    
    [
        #var(type:vint)aRetVal.
        $self indexOf &index:aPosition &char:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
    
    #method indexOf &index:aPosition &literal:aValue
    [
        #var(type:vint)aRetVal.
        $self indexOf &index:aPosition &literal:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
    
    #method indexOf &index:aPosition &wide:aValue
    [
        #var(type:vint)aRetVal.
        $self indexOf &index:aPosition &wide:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method(type:literal) Substring &index:anIndex &length:aLength
    [
        (aLength == 0)
            ? [ ^ emptyLiteralValue. ].
            
        ^ LiteralValue new &length:aLength &index:anIndex &chararray:theBuffer.
    ]

    #method(type:wide) WideSubstring &index:anIndex &length:aLength
    [
        (aLength == 0)
            ? [ ^ emptyWideLiteralValue. ].
            
        ^ WideLiteralValue new &length:aLength &index:anIndex &chararray:theBuffer.
    ]

    #method delete &index:anIndex &length:aLength
    [
        #var(type:int)n := 0 - aLength.
        
        theBuffer move &index:anIndex &length:theLength &offset:n.
        
        theLength -= aLength.
    ]
}
