// ==== ver 3.4.1 ===

#import system.
#import system'text.

// --- Statemachine ---

#subject(class:Statemachine)token_dfa.

#class(limited) Statemachine
{
    #method(type:bool,stacksafe) proceed &char:aChar = $nil.
    
    #method(type:bool) is &whitespace = $nil.
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

