import system'text.

// --- Statemachine ---

public interface Statemachine
{
    abstract stacksafe bool proceed(char aChar) []
    
    abstract bool isWhitespace []
    
//    token_dfa = $self.
    
//    state = $nil.
}

public class SimpleStatemachine :: Statemachine
{
    bool theSpaceMode.
    
    constructor new
    [
        theSpaceMode := true.
    ]
    
    stacksafe bool proceed(char aChar)
    [
        if (aChar isWhitespace)
            [
                ifnot (theSpaceMode)
                    [ theSpaceMode := true. ^ true ]
            ];
            [
                theSpaceMode := false
            ].
            
        ^ false
    ]
    
    bool isWhitespace = theSpaceMode.
}

//class ExpressionStatemachine :: Statemachine
//{
//    bool theSpaceMode.
//    bool theQuoteMode.
//    bool theNewToken.
//    bool theOperatorMode.
//    
//    constructor new
//    [
//        theSpaceMode := true.
//        theQuoteMode := false.
//        theNewToken := true.
//        theOperatorMode := false.
//    ]
//    
//    bool proceed char:aChar
//    [
//        if (theQuoteMode)
//            [
//                if (aChar == $22h)
//                    [ theQuoteMode := false. theSpaceMode := false. theNewToken := false. theOperatorMode := true ].
//                    
//                ^ false
//            ].
//        
//        bool whitespace := aChar isWhitespace.
//        bool digit := aChar isDigit.
//        bool letter := aChar isLToken.
//        if (whitespace)
//            [
//                ifnot(theSpaceMode)
//                    [ theSpaceMode := true. theNewToken := true. ^ true ]
//            ];
//            [
//                if (aChar == $22h)
//                    [
//                        ifnot(theNewToken)
//                            [ theNewToken := true. ^ true ].
//                        
//                        theQuoteMode := true.
//                        theSpaceMode := false.
//                        
//                        ^ false
//                    ].
//                                      
//                if (theNewToken)
//                    [ 
//                        theNewToken := false.
//                        theSpaceMode := false. 
//                        if (letter or:digit)
//                            [ theOperatorMode := false ];
//                            [ theOperatorMode := true ].
//                    ];
//                    [                
//                        if (letter or:digit)
//                            [
//                                if(theOperatorMode)
//                                    [ theNewToken := true. theOperatorMode := false. ^ true ]
//                            ];
//                            [ 
//                                theOperatorMode := true.
//                                theNewToken := true. ^ true
//                            ]
//                    ]
//            ].
//        
//        ^ false
//    ]
//    
//    bool isWhitespace = theSpaceMode.    
//}

public class TokenStatemachine :: Statemachine
{
    bool theSpaceMode.
    bool theQuoteMode.
    bool theNewToken.
    
    constructor new
    [
        theSpaceMode := true.
        theQuoteMode := false.
        theNewToken := true.
    ]
    
    bool proceed(char aChar)
    [
        if (theQuoteMode)
            [
                if (aChar == $22h)
                    [ theQuoteMode := false. theSpaceMode := false. theNewToken := false ].
                    
                ^ false
            ].
        
        bool whitespace := aChar isWhitespace.
        if (whitespace)
            [
                ifnot(theSpaceMode)
                    [ theSpaceMode := true. theNewToken := true. ^ true ]
            ];
            [
                if (aChar == $22h)
                    [
                        ifnot(theNewToken)
                            [ theNewToken := true. ^ true ].
                        
                        theQuoteMode := true.
                        theSpaceMode := false.
                        
                        ^ false
                    ].
                                      
                if (theNewToken)
                    [ 
                        theNewToken := false.
                        theSpaceMode := false. 
                    ].
            ].
        
        ^ false
    ]
    
    bool isWhitespace = theSpaceMode.    
}

public class TokenEnumerator :: Enumerator<literal>, interface<Enumerator>
{
    literal         theLiteral.
    ref<int>        theIndex.
    T<TextBuilder>  theBuffer.    
    T<Statemachine> theDFA.
    
    constructor new(literal aLiteral, Statemachine dfa)
    [
        theLiteral := aLiteral.
        theIndex := 0.
        theBuffer := TextBuilder new.
        theDFA := dfa.
    ]    
    
    constructor new(literal aLiteral)
        <= new(aLiteral, SimpleStatemachine new).
    
    constructor new : val
        <= new(val literal, SimpleStatemachine new).
    
    reset 
    [
        theIndex value := 0.
    ]
    
    enumerable = theLiteral.
    
    bool next
    [
        theBuffer clear.

        int  aLength := 0.
        int  aCurrent := theIndex.
        char aChar := $0.
        int  aCharLen := 0.
        
        theLiteral readLengthTo(&aLength).
        
        while (aCurrent < aLength)
        [
            theLiteral read(aCurrent, &aChar).
            
            if (theDFA proceed(aChar))
                [
                    theIndex value := aCurrent.
                    
                    ^ true
                ];
                [
                    ifnot (theDFA isWhitespace)
                        [ theBuffer write(aChar) ]
                ].
                
            aChar readLengthTo(&aCharLen).
            aCurrent := aCurrent + aCharLen
        ].

        theIndex value := aCurrent.

        ^ theBuffer isNonempty
    ]
    
    literal get = theBuffer literal.
}
