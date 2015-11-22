// ==== ver 3.4.1 ===

#import system. 

#symbol(const,type:int)$TextPageSize = 128.

#subject(class:system'text'TextBuffer) textbuffer.

// --- TextBuffer ---

#class(sealed) TextBuffer
{
    #field(type:chararray) theBuffer.
    #field(type:vint)      theLength.

    #constructor new
    [
        theBuffer := CharArray new &int:$TextPageSize.
        theLength := Integer new &int:0.
    ]

    #method clear
    [
        theLength << 0.
    ]

    #method(stacksafe) readLength &vint:aRetVal
    [
        aRetVal << theLength.
    ]

    #method length
        = theLength int.

    #method(stacksafe) setLength &int:aLength
    [
        theLength << aLength.
    ]

    #method set &length:aLength
        <= setLength &int:(aLength int).

    #method chararray = theBuffer.

    #method literal
        = LiteralValue new &int:0 &int:theLength &chararray:theBuffer.

    #method wide
        = WideLiteralValue new &int:0 &int:theLength &chararray:theBuffer.

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
                                    
    #method(stacksafe) reserve &int:aLength
    [
        #var(type:int)aNewLength := aLength.
        
        aNewLength := aNewLength + theLength.
        
        #var(type:int)aBufferLength.
        theBuffer readLength &vint:aBufferLength.
        
        (aNewLength >= aBufferLength) ?
        [
            aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            
            #var(type:chararray) aNewBuffer := CharArray new &int:aNewLength.
            
            aNewBuffer write &int:0 &int:aBufferLength &chararray:theBuffer.
            
            theBuffer := aNewBuffer.
        ].
    ]

    #method write &literal:aLiteral
    [
        #var(type:int)aLength.
        aLiteral readLength &vint:aLength.
        
        (0 == aLength) !
        [
            $self reserve &int:aLength.
            
            aLiteral save &int:theLength &chararray:theBuffer &vint:aLength.
            
            theLength += aLength.
        ].
    ]

    #method write &wide:aLiteral
    [
        #var(type:int)aLength.
        aLiteral readLength &vint:aLength.
        
        (0 == aLength) !
        [
            $self reserve &int:aLength.
            
            aLiteral save &int:theLength &chararray:theBuffer &vint:aLength.
            
            theLength += aLength.
        ].
    ]

    #method(stacksafe) write &char:aChar
    [
        $self reserve &int:1.
        
        theBuffer write &int:theLength &char:aChar.
        
        theLength += 1.
    ]

    #method(generic) write : anObject
    [
        $self write &literal:(anObject literal).
    ]

    #method(stacksafe) write &chararray:anArray &int:aLength
    [
        (0 == aLength) !
        [
            $self reserve &int:aLength.
            
            theBuffer write &int:theLength &int:aLength &chararray:anArray.
            
            theLength += aLength.
        ].
    ]

    #method insert &int:anIndex &int:aLength &chararray:anArray
    [
        $self reserve &int:aLength.
        
        theBuffer move &int:anIndex &int:theLength &int:aLength.
        
        theBuffer write &int:anIndex &int:aLength &chararray:anArray.
        
        theLength += aLength.
    ]

    #method insert &int:anIndex &literal:aLiteral
    [
        #var(type:int)aLength.
        UTF8Encoding charCount &literal:aLiteral &vint:aLength.

        $self reserve &int:aLength.
        
        theBuffer move &int:anIndex &int:theLength &int:aLength.
        
        aLiteral save &int:anIndex &chararray:theBuffer &vint:aLength.
        
        theLength += aLength.
    ]

    #method insert &int:anIndex &wide:aLiteral
    [
        #var(type:int)aLength.
        UTF16Encoding charCount &wide:aLiteral &vint:aLength.

        $self reserve &int:aLength.
        
        theBuffer move &int:anIndex &int:theLength &int:aLength.
        
        aLiteral save &int:anIndex &chararray:theBuffer &vint:aLength.
        
        theLength += aLength.
    ]
    
    #method insert &int:anIndex &char:aChar
    [
        $self reserve &int:1.
        
        theBuffer move &int:anIndex &int:theLength &int:1.
        
        theBuffer write &int:anIndex &char:aChar.
        
        theLength += 1.
    ]

    #method(stacksafe) indexOf &int:aPosition &char:aValue &vint:aRetVal
    [
        #var(type:int)anIndex := 0.
        #var(type:char)aCurrent.
        
        #loop (anIndex < theLength)?
        [
            theBuffer read &int:anIndex &vchar:aCurrent.
            
            (aCurrent == aValue)
                ? [ aRetVal << anIndex. ^$self. ].
                
            anIndex := anIndex + 1.
        ].
        
        aRetVal << -1.
    ]

    #method(stacksafe) indexOf &int:aPosition &literal:aLiteral &vint:aRetVal
    [
        #var(type:int)aLiteralLength.
        #var(type:int)anIndex := 0.
        #var(type:int)aFirstCharLen.
        #var(type:char)aCurrent.
        #var(type:char)aFirstChar.
        #var(type:char)aNextChar.
        
        aLiteral readLength &vint:aLiteralLength.
        
        aLiteral read &int:0 &vchar:aFirstChar.
        aFirstChar readLength &vint:aFirstCharLen.
        
        #var(type:int)aMaxIndex := theLength - aLiteralLength.
        #loop (anIndex < aMaxIndex)?
        [
            theBuffer read &int:anIndex &vchar:aCurrent.
            
            (aCurrent == aFirstChar) ?
            [ 
                #var(type:int)j := aFirstCharLen.
                #var(type:int)chLen.
                #loop (j < aLiteralLength) ?
                [
                    theBuffer read &int:(anIndex + j) &vchar:aCurrent.
                    aLiteral read &int:j &vchar:aNextChar.
                    
                    (aNextChar != aCurrent)
                        ? [ j := aLiteralLength. chLen := 1. ]
                        ! [ aNextChar readLength &vint:chLen. ].
                    
                    j := j + chLen.
                ].
                
                (j == aLiteralLength)
                    ? [ aRetVal << anIndex. ^ $self. ].
            ].
                
            anIndex := anIndex + 1.
        ].
        
        aRetVal << -1.
    ]

    #method(stacksafe) indexOf &int:aPosition &wide:aLiteral &vint:aRetVal
    [
        #var(type:int)aLiteralLength.
        #var(type:int)anIndex := 0.
        #var(type:int)aFirstCharLen.
        #var(type:char)aCurrent.
        #var(type:char)aFirstChar.
        #var(type:char)aNextChar.
        
        aLiteral readLength &vint:aLiteralLength.
        
        aLiteral read &int:0 &vchar:aFirstChar.
        aFirstChar readWideLength &vint:aFirstCharLen.
        
        #loop (anIndex < theLength)?
        [
            theBuffer read &int:anIndex &vchar:aCurrent.
            
            (aCurrent == aFirstChar) ?
            [ 
                #var(type:int)j := aFirstCharLen.
                #var(type:int)chLen.
                #loop (j < aLiteralLength) ?
                [
                    theBuffer read &int:(anIndex + j) &vchar:aCurrent.
                    aLiteral read &int:j &vchar:aNextChar.
                    
                    (aNextChar != aCurrent)
                        ? [ j := aLiteralLength. chLen := 1. ]
                        ! [ aNextChar readWideLength &vint:chLen. ].
                    
                    j := j + chLen.
                ].
                
                (j > aLiteralLength)
                    ? [ aRetVal << anIndex. ^ $self. ].
            ].
                
            anIndex := anIndex + 1.
        ].
        
        aRetVal << -1.
    ]

    #method indexOf &int:aPosition &char:aValue    
    [
        #var(type:int)aRetVal.
        $self indexOf &int:aPosition &char:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
    
    #method indexOf &int:aPosition &literal:aValue
    [
        #var(type:int)aRetVal.
        $self indexOf &int:aPosition &literal:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
    
    #method indexOf &int:aPosition &wide:aValue
    [
        #var(type:int)aRetVal.
        $self indexOf &int:aPosition &wide:aValue &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method(type:literal,stacksafe) Substring &int:anIndex &int:aLength
    [
        (aLength == 0)
            ? [ ^ emptyLiteralValue. ].
            
        ^ LiteralValue new &int:anIndex &int:aLength &chararray:theBuffer.
    ]

    #method(type:wide,stacksafe) WideSubstring &int:anIndex &int:aLength
    [
        (aLength == 0)
            ? [ ^ emptyWideLiteralValue. ].
            
        ^ WideLiteralValue new &int:anIndex &int:aLength &chararray:theBuffer.
    ]

    #method(stacksafe) delete &int:anIndex &int:aLength
    [
        #var(type:int)n := 0 - aLength.
        #var(type:int)l := theLength.
        l := l - anIndex.
        l := l - aLength.
        
        theBuffer move &int:(anIndex + aLength) &int:l &int:n.
        
        theLength -= aLength.
    ]
}
