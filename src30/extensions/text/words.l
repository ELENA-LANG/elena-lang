#import system.
#import system'text.

// --- Statemachine ---

#subject token_dfa = Statemachine.

#class(limited) Statemachine
{
    #method(bool,stacksafe) proceed &char:aChar = $nil.
    
    #method(bool) is &whitespace = $nil.
    
    #method token_dfa = $self.
    
    #method state = $nil.
}

#class SimpleStatemachine :: Statemachine
{
    #field(bool)theSpaceMode.
    
    #constructor new
    [
        theSpaceMode := true.
    ]
    
    #method proceed &char:aChar
    [
        (aChar is &whitespace)
            ? [
                theSpaceMode
                    ! [ theSpaceMode := true. ^ true. ].
            ]
            ! [
                theSpaceMode := false.
            ].
            
        ^ false.            
    ]
    
    #method is &whitespace = theSpaceMode.
}

#class ExpressionStatemachine :: Statemachine
{
    #field(bool)theSpaceMode.
    #field(bool)theQuoteMode.
    #field(bool)theNewToken.
    #field(bool)theOperatorMode.
    
    #constructor new
    [
        theSpaceMode := true.
        theQuoteMode := false.
        theNewToken := true.
        theOperatorMode := false.
    ]
    
    #method proceed &char:aChar
    [
        theQuoteMode?
            [
                (aChar == #22h)
                    ? [ theQuoteMode := false. theSpaceMode := false. theNewToken := false. theOperatorMode := true. ].
                    
                ^ false.                
            ].
        
        #var(bool)whitespace := aChar is &whitespace.
        #var(bool)digit := aChar is &digit.
        #var(bool)letter := aChar is &ltoken.
        whitespace
            ? [
                theSpaceMode
                    ! [ theSpaceMode := true. theNewToken := true. ^ true. ].
            ]
            ! [
                (aChar == #22h)
                    ? [
                        theNewToken
                            ! [ theNewToken := true. ^ true. ].
                        
                        theQuoteMode := true.
                        theSpaceMode := false.
                        
                        ^ false.
                      ].
                                      
                theNewToken
                    ? [ 
                        theNewToken := false.
                        theSpaceMode := false. 
                        (letter or:digit)
                            ? [ theOperatorMode := false. ]
                            ! [ theOperatorMode := true. ].
                      ]
                    ! [                
                        (letter or:digit)
                            ? [
                                theOperatorMode
                                    ? [ theNewToken := true. theOperatorMode := false. ^ true. ].
                            ]
                            ! [ 
                                theOperatorMode := true.
                                theNewToken := true. ^ true.
                            ].
                    ].
            ].
        
        ^ false.
    ]
    
    #method is &whitespace = theSpaceMode.    
}

/// obosolet
#symbol WordEnumerator = TokenEnumerator.

#class TokenEnumerator :: Enumerator
{
    #field(literal)    theLiteral.
    #field(vint)       theIndex.
    #field(textbuffer) theBuffer.
    
    #field(token_dfa)  theDFA.
    
    #constructor(suppress:w3) new &literal:aLiteral &token_dfa:dfa
    [
        theLiteral := aLiteral.
        theIndex := 0.
        theBuffer := TextBuilder new.
        theDFA := dfa.
    ]    
    
    #constructor new &literal:aLiteral
        <= new &literal:aLiteral &token_dfa:(SimpleStatemachine new).
    
    #method reset 
    [
        theIndex set &int:0.
    ]
    
    #method next
    [
        theBuffer clear.

        #var(int)aLength.
        #var(int)aCurrent := theIndex.
        #var(char)aChar.
        #var(int)aCharLen.
        
        theLiteral readLength &vint:aLength.
        
        #loop (aCurrent < aLength) ?
        [
            theLiteral read &int:aCurrent &vchar:aChar.
            
            (theDFA proceed &char:aChar)
                ? [
                    theIndex set &int:aCurrent.
                    
                    ^ true.
                ]
                ! [
                    (theDFA is &whitespace)
                        ! [ theBuffer write &char:aChar. ].
                ].
                
            aChar readLength &vint:aCharLen.
            aCurrent := aCurrent + aCharLen.
        ].

        theIndex set &int:aCurrent.

        ^ theBuffer is &nonempty.
    ]
    
    #method get = theBuffer literal.
}

