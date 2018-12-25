//import system'text.

// --- Statemachine ---

public interface Statemachine
{
    abstract bool proceed(char ch) {}
    
    abstract bool isWhitespace() {}
}

public class SimpleStatemachine : Statemachine
{
    bool theSpaceMode;
    
    constructor()
    {
        theSpaceMode := true
    }
    
    bool proceed(char ch)
    {
        if (ch.isWhitespace())
        {
            ifnot (theSpaceMode)
                { theSpaceMode := true; ^ true }
        }
        else
        {
            theSpaceMode := false
        };
            
        ^ false
    }
    
    bool isWhitespace() = theSpaceMode;
}

//public class ExpressionStatemachine :: Statemachine
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
//    bool proceed(char aChar)
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
//                        if (letter || digit)
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
//
//public class TokenStatemachine :: Statemachine
//{
//    bool theSpaceMode.
//    bool theQuoteMode.
//    bool theNewToken.
//    
//    constructor new
//    [
//        theSpaceMode := true.
//        theQuoteMode := false.
//        theNewToken := true.
//    ]
//    
//    bool proceed(char aChar)
//    [
//        if (theQuoteMode)
//            [
//                if (aChar == $22h)
//                    [ theQuoteMode := false. theSpaceMode := false. theNewToken := false ].
//                    
//                ^ false
//            ].
//        
//        bool whitespace := aChar isWhitespace.
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
//                    ].
//            ].
//        
//        ^ false
//    ]
//    
//    bool isWhitespace = theSpaceMode.    
//}

public class TokenEnumerator : Enumerator<string>, interface<Enumerator>
{
    string       theLiteral;
    ref<int>     theIndex;
    TextBuilder  theBuffer;
    Statemachine theDFA;
    
    constructor(string s, Statemachine dfa)
    {
        theLiteral := s;
        theIndex := 0;
        theBuffer := new TextBuilder();
        theDFA := dfa;
    }    
    
    constructor(string s)
        <= (s, new SimpleStatemachine());
    
    constructor new(o)
        <= (cast String(o), new SimpleStatemachine());
    
    reset()
    {
        theIndex.Value := 0;
    }
    
    enumerable() = theLiteral;
    
    bool next()
    {
        theBuffer.clear();

        int  len := theLiteral.Length;
        int  current := theIndex;
        char ch := $0;
        int  chLen := 0;
        
        while (current < len)
        {
            theLiteral.read(current, ref ch);
            
            if (theDFA.proceed(ch))
            {
                theIndex.Value := current;
                
                ^ true
            }
            else
            {
                ifnot (theDFA.isWhitespace())
                    { theBuffer.write(ch) }
            };
                
            ch.readLengthTo(ref chLen);
            current := current + chLen
        };

        theIndex.Value := current;

        ^ theBuffer.isNonempty()
    }
    
    string get() = theBuffer.Value;
}
