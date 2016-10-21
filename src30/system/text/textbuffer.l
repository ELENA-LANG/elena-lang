#import system. 

#symbol(const,int)$TextPageSize = 128.

#subject textbuffer = system'text'TextBuilder.

// --- TextBuffer ---

#class(sealed,nonstructural) TextBuilder
{
    #field(chararray) theBuffer.
    #field(vint)      theLength.

    #constructor(w3) new
    [
        theBuffer := CharArray new &int:$TextPageSize.
        theLength := 0.
    ]

//    #method clear
//    [
//        theLength set &int:0.
//    ]

    #method(stacksafe,outnprop:length) readLength &vint:aRetVal
    [
        aRetVal set &int:theLength.
    ]

    #method(stacksafe) setLength &int:aLength
    [
        theLength set &int:aLength.
    ]

    #method set &length:aLength
        <= setLength &int:(aLength int).

//    #method chararray = theBuffer.

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
        #var(int)aNewLength := aLength.
        
        aNewLength := aNewLength + theLength.
        
        #var(int)aBufferLength := theBuffer length.
        
        (aNewLength >= aBufferLength) ?
        [
            //aNewLength := (aNewLength / $TextPageSize + 1) * $TextPageSize.
            aNewLength /= $TextPageSize.
            aNewLength += 1.
            aNewLength *= $TextPageSize.            
            
            #var(chararray) aNewBuffer := CharArray new &int:aNewLength.
            
            aNewBuffer write &int:0 &int:aBufferLength &chararray:theBuffer.
            
            theBuffer := aNewBuffer.
        ].
    ]

    #method write &literal:aLiteral
    [
        #var(int)aLength := aLiteral length.
        
        (0 == aLength) !
        [
            $self reserve &int:aLength.
            
            aLiteral save &int:theLength &chararray:theBuffer &vint:aLength.
            
            theLength += aLength.
        ].
    ]

    #method write &wide:aLiteral
    [
        #var(int)aLength := aLiteral length.
        
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

//    #method(stacksafe) write &chararray:anArray &int:aLength
//    [
//        (0 == aLength) !
//        [
//            $self reserve &int:aLength.
//            
//            theBuffer write &int:theLength &int:aLength &chararray:anArray.
//            
//            theLength += aLength.
//        ].
//    ]
//
//    #method insert &int:anIndex &int:aLength &chararray:anArray
//    [
//        $self reserve &int:aLength.
//        
//        theBuffer move &int:anIndex &int:theLength &int:aLength.
//        
//        theBuffer write &int:anIndex &int:aLength &chararray:anArray.
//        
//        theLength += aLength.
//    ]
//
//    #method insert &int:anIndex &literal:aLiteral
//    [
//        #var(int)aLength.
//        UTF8Encoding charCount &literal:aLiteral &vint:aLength.
//
//        $self reserve &int:aLength.
//        
//        theBuffer move &int:anIndex &int:theLength &int:aLength.
//        
//        aLiteral save &int:anIndex &chararray:theBuffer &vint:aLength.
//        
//        theLength += aLength.
//    ]
//
//    #method insert &int:anIndex &wide:aLiteral
//    [
//        #var(int)aLength.
//        UTF16Encoding charCount &wide:aLiteral &vint:aLength.
//
//        $self reserve &int:aLength.
//        
//        theBuffer move &int:anIndex &int:theLength &int:aLength.
//        
//        aLiteral save &int:anIndex &chararray:theBuffer &vint:aLength.
//        
//        theLength += aLength.
//    ]
//    
//    #method insert &int:anIndex &char:aChar
//    [
//        $self reserve &int:1.
//        
//        theBuffer move &int:anIndex &int:theLength &int:1.
//        
//        theBuffer write &int:anIndex &char:aChar.
//        
//        theLength += 1.
//    ]
//
//    #method(stacksafe) indexOf &int:aPosition &char:aValue &vint:aRetVal
//    [
//        #var(int)anIndex := 0.
//        #var(char)aCurrent.
//        
//        #loop (anIndex < theLength)?
//        [
//            theBuffer read &int:anIndex &vchar:aCurrent.
//            
//            (aCurrent == aValue)
//                ? [ aRetVal := anIndex. ^$self. ].
//                
//            anIndex += 1.
//        ].
//        
//        aRetVal := -1.
//    ]
//
//    #method(stacksafe) indexOf &int:aPosition &literal:aLiteral &vint:aRetVal
//    [
//        #var(int)aLiteralLength.
//        #var(int)anIndex := 0.
//        #var(int)aFirstCharLen.
//        #var(char)aCurrent.
//        #var(char)aFirstChar.
//        #var(char)aNextChar.
//        
//        aLiteral readLength &vint:aLiteralLength.
//        
//        aLiteral read &int:0 &vchar:aFirstChar.
//        aFirstChar readLength &vint:aFirstCharLen.
//        
//        #var(int)aMaxIndex := theLength.
//        aMaxIndex -= aLiteralLength.
//        
//        #loop (anIndex < aMaxIndex)?
//        [
//            theBuffer read &int:anIndex &vchar:aCurrent.
//            
//            (aCurrent == aFirstChar) ?
//            [ 
//                #var(int)j := aFirstCharLen.
//                #var(int)chLen.
//                #loop (j < aLiteralLength) ?
//                [
//                    theBuffer read &int:(anIndex + j) &vchar:aCurrent.
//                    aLiteral read &int:j &vchar:aNextChar.
//                    
//                    (aNextChar == aCurrent)
//                        ? [ aNextChar readLength &vint:chLen. ]
//                        ! [ j := aLiteralLength. chLen := 1. ].
//                    
//                    j := j + chLen.
//                ].
//                
//                (j == aLiteralLength)
//                    ? [ aRetVal := anIndex. ^ $self. ].
//            ].
//                
//            anIndex := anIndex + 1.
//        ].
//        
//        aRetVal := -1.
//    ]
//
//    #method(stacksafe) indexOf &int:aPosition &wide:aLiteral &vint:aRetVal
//    [
//        #var(int)aLiteralLength.
//        #var(int)anIndex := 0.
//        #var(int)aFirstCharLen.
//        #var(char)aCurrent.
//        #var(char)aFirstChar.
//        #var(char)aNextChar.
//        
//        aLiteral readLength &vint:aLiteralLength.
//        
//        aLiteral read &int:0 &vchar:aFirstChar.
//        aFirstChar readWideLength &vint:aFirstCharLen.
//        
//        #loop (anIndex < theLength)?
//        [
//            theBuffer read &int:anIndex &vchar:aCurrent.
//            
//            (aCurrent == aFirstChar) ?
//            [ 
//                #var(int)j := aFirstCharLen.
//                #var(int)chLen.
//                #loop (j < aLiteralLength) ?
//                [
//                    theBuffer read &int:(anIndex + j) &vchar:aCurrent.
//                    aLiteral read &int:j &vchar:aNextChar.
//                    
//                    (aNextChar == aCurrent)
//                        ? [ aNextChar readWideLength &vint:chLen. ]
//                        ! [ j := aLiteralLength. chLen := 1. ].
//                    
//                    j := j + chLen.
//                ].
//                
//                (j > aLiteralLength)
//                    ? [ aRetVal := anIndex. ^ $self. ].
//            ].
//                
//            anIndex := anIndex + 1.
//        ].
//        
//        aRetVal := -1.
//    ]
//
//    #method indexOf &int:aPosition &char:aValue    
//    [
//        #var(int)aRetVal.
//        $self indexOf &int:aPosition &char:aValue &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//    
//    #method indexOf &int:aPosition &literal:aValue
//    [
//        #var(int)aRetVal.
//        $self indexOf &int:aPosition &literal:aValue &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//    
//    #method indexOf &int:aPosition &wide:aValue
//    [
//        #var(int)aRetVal.
//        $self indexOf &int:aPosition &wide:aValue &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//
//    #method(literal,stacksafe) Substring &int:anIndex &int:aLength
//    [
//        (aLength == 0)
//            ? [ ^ emptyLiteralValue. ].
//            
//        ^ LiteralValue new &int:anIndex &int:aLength &chararray:theBuffer.
//    ]
//
//    #method(wide,stacksafe) WideSubstring &int:anIndex &int:aLength
//    [
//        (aLength == 0)
//            ? [ ^ emptyWideLiteralValue. ].
//            
//        ^ WideLiteralValue new &int:anIndex &int:aLength &chararray:theBuffer.
//    ]
//
//    #method(stacksafe) delete &int:anIndex &int:aLength
//    [
//        #var(int)n := 0 - aLength.
//        #var(int)l := theLength.
//        l := l - anIndex.
//        l := l - aLength.
//        
//        theBuffer move &int:(anIndex + aLength) &int:l &int:n.
//        
//        theLength -= aLength.
//    ]
}

#symbol TextBuffer = TextBuilder.