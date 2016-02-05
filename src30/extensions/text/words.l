// ==== ver 3.4.1 ===

#import system.
#import system'text.

// --- Statemachine ---

#subject(class:Statemachine)token_dfa.

#class(limited) Statemachine
{
    #method(type:bool,stacksafe) proceed &char:aChar = $nil.
    
    #method(type:bool) is &whitespace = $nil.
    
    #method token_dfa = $self.
    
    #method state = $nil.
}

#class SimpleStatemachine :: Statemachine
{
    #field(type:bool)theSpaceMode.
    
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
    #field(type:bool)theSpaceMode.
    #field(type:bool)theQuoteMode.
    #field(type:bool)theNewToken.
    #field(type:bool)theOperatorMode.
    
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
        
        #var(type:bool)whitespace := aChar is &whitespace.
        #var(type:bool)digit := aChar is &digit.
        #var(type:bool)letter := aChar is &letter.
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
    #field(type:literal)    theLiteral.
    #field(type:vint)       theIndex.
    #field(type:textbuffer) theBuffer.
    
    #field(type:token_dfa)  theDFA.
    
    #constructor new &literal:aLiteral &token_dfa:dfa
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
        theBuffer := TextBuffer new.
        theDFA := dfa.
    ]    
    
    #constructor new &literal:aLiteral
        <= new &literal:aLiteral &token_dfa:(SimpleStatemachine new).
    
    #method reset 
    [
        theIndex write &int:0.
    ]
    
    #method next
    [
        theBuffer clear.

        #var(type:int)aLength.
        #var(type:int)aCurrent := theIndex.
        #var(type:char)aChar.
        #var(type:int)aCharLen.
        
        theLiteral readLength &vint:aLength.
        
        #loop (aCurrent < aLength) ?
        [
            theLiteral read &int:aCurrent &vchar:aChar.
            
            (theDFA proceed &char:aChar)
                ? [
                    theIndex write &int:aCurrent.
                    
                    ^ true.
                ]
                ! [
                    (theDFA is &whitespace)
                        ! [ theBuffer write &char:aChar. ].
                ].
                
            aChar readLength &vint:aCharLen.
            aCurrent := aCurrent + aCharLen.
        ].

        theIndex write &int:aCurrent.

        ^ theBuffer is &nonempty.
    ]
    
    #method get = theBuffer literal.
}

