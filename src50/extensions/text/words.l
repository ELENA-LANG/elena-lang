import system'text;

namespace text
{
    // --- Statemachine ---
    
    public interface Statemachine
    {
        abstract bool proceed(char ch);
        
        abstract bool isWhitespace();
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
    
    public class ExpressionStatemachine : Statemachine
    {
        bool theSpaceMode;
        bool theQuoteMode;
        bool theNewToken;
        bool theOperatorMode;
        
        constructor()
        {
            theSpaceMode := true;
            theQuoteMode := false;
            theNewToken := true;
            theOperatorMode := false
        }
        
        bool proceed(char ch)
        {
            if (theQuoteMode)
            {
                if (ch == $22h)
                    { theQuoteMode := false; theSpaceMode := false; theNewToken := false; theOperatorMode := true };
                    
                ^ false
            };
            
            bool whitespace := ch.isWhitespace();
            bool digit := ch.isDigit();
            bool letter := ch.isLToken();
            if (whitespace)
            {
                ifnot(theSpaceMode)
                    { theSpaceMode := true; theNewToken := true; ^ true }
            }
            else
            {
                if (ch == $22h)
                {
                    ifnot(theNewToken)
                        { theNewToken := true; ^ true };
                    
                    theQuoteMode := true;
                    theSpaceMode := false;
                    
                    ^ false
                };
                
                if (theNewToken)
                { 
                    theNewToken := false;
                    theSpaceMode := false; 
                    theOperatorMode := (letter || digit) ? false : true
                }
                else
                {                
                    if (letter || digit)
                    {
                        if(theOperatorMode)
                            { theNewToken := true; theOperatorMode := false; ^ true }
                    }
                    else
                    { 
                        theOperatorMode := true;
                        theNewToken := true; ^ true
                    }
                }                  
            };
            
            ^ false
        }
        
        bool isWhitespace() = theSpaceMode;    
    }
    
    public class TokenStatemachine : Statemachine
    {
        bool theSpaceMode;
        bool theQuoteMode;
        bool theNewToken;
        
        constructor()
        {
            theSpaceMode := true;
            theQuoteMode := false;
            theNewToken := true
        }
        
        bool proceed(char ch)
        {
            if (theQuoteMode)
            {
                if (ch == $22h)
                   { theQuoteMode := false; theSpaceMode := false; theNewToken := false };
                    
                ^ false
            };
            
            bool whitespace := ch.isWhitespace();
            if (whitespace)
            {
                ifnot(theSpaceMode)
                    { theSpaceMode := true; theNewToken := true; ^ true }
            }
            else
            {
                if (ch == $22h)
                {
                    ifnot(theNewToken)
                        { theNewToken := true; ^ true };
                    
                    theQuoteMode := true;
                    theSpaceMode := false;
                    
                    ^ false
                };
                                      
                if (theNewToken)
                { 
                    theNewToken := false;
                    theSpaceMode := false 
                }
            };
            
            ^ false
        }
        
        bool isWhitespace() = theSpaceMode;    
    }
    
    public class TokenEnumerator : Enumerator<string>, interface<Enumerator>
    {
        string       theLiteral;
        ref<int>     theIndex;
        TextBuilder  theBuffer;
        Statemachine theDFA;
    
        constructor()
        {
            theLiteral := String.MinValue;
            theIndex := 0;
            theBuffer := new TextBuilder();
            theDFA := new SimpleStatemachine();
        }    
            
        constructor(string s, Statemachine dfa)
        {
            theLiteral := s;
            theIndex := 0;
            theBuffer := new TextBuilder();
            theDFA := dfa;
        }    
        
        constructor new(s)
        {
            theLiteral := cast string(s);
        }
        
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
                ch := theLiteral[current];
                
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
                    
                chLen := ch.Length;
                current := current + chLen
            };
    
            theIndex.Value := current;
    
            ^ theBuffer.isNonempty()
        }
        
        get string Value() = *theBuffer;
    }

}