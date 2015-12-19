#import system.
#import system'dynamic.
#import system'routines.
#import system'collections.
#import system'io.
#import extensions.
#import extensions'routines.
#import extensions'text.
#import system'culture.

// --- Lexer ---

#symbol(type:int,const)dfaIdentifier = 1.
#symbol(type:int,const)dfaDigit = 2.
#symbol(type:int,const)dfaLiteral = 3.
#symbol(type:int,const)dfaOther = 4.
#symbol(type:int,const)dfaEOF = 5.

#class LexerDFA :: Statemachine
{
    #field(type:bool)theSpaceMode.
    #field(type:bool)theSingleTokenMode.
    #field(type:bool)theQuoteMode.
    #field(type:bool)theNewToken.
    
    #field theTokenType.
    
    #constructor new
    [
        theSpaceMode := true.
        theSingleTokenMode := false.
        theNewToken := true.
        theQuoteMode := false.
    ]
    
    #method state = theTokenType.
    
    #method proceed &char:aChar
    [
        theQuoteMode?
            [
                (aChar == #22h)
                    ? [ theQuoteMode := false. theSingleTokenMode := true. ].
                    
                ^ false.                
            ].
        
        #var(type:bool)whitespace := aChar is &whitespace.
        #var(type:bool)digit := aChar is &digit.
        #var(type:bool)letter := aChar is &letter.
        
        whitespace
            ? [
                theSingleTokenMode := false.
                                
                theSpaceMode
                    ! [ theSpaceMode := true. ^ true. ].
            ]
            ! [
                theSingleTokenMode
                    ? [  theSingleTokenMode := false. theSpaceMode := false. theNewToken := true. ^ true. ].
                
                (letter or:digit)
                    ! [ 
                        theSpaceMode
                            ! [ theSpaceMode := true. ^ true. ].
                        
                        (aChar == #22h)
                            ? [ 
                                theTokenType := dfaLiteral. 
                                theNewToken := false. 
                                theQuoteMode := true. 
                              ]
                            ! [ theSingleTokenMode := true. ].
                    ].
                    
                theSpaceMode ?
                    [ theSpaceMode := false. theNewToken := true. ].

                theNewToken ?
                [
                    theTokenType := digit iif:dfaDigit:(letter iif:dfaIdentifier:dfaOther).
                    
                    theNewToken := false.
                ].
            ].
        
        ^ false.            
    ]
    
    #method is &whitespace = theSpaceMode.    
}

#symbol registerState = (:token:state:lineNumber:output)
[
    (state == dfaIdentifier)
        ? [
            #var ch1 := token@0 literal toLower &locale:currentLocale.
            #var num := control try:[ token literal Substring &at:1 toInt ] &catching:e [ feedback raiseError:"Invalid register name". ].
    
            (((ch1 == "r") || (ch1 == "s") || (ch1 == "d")) && (num < 15))
                ! [ feedback raiseError:"Register ":token:" is not valid". ].
                            
            (ch1 == "s") 
                ? [ num := num + 16. ].
            (ch1 == "d")
                ? [ num := num + 32. ].
                
            output += assembler'Tokens'TokenRegister new &register:num &line:lineNumber.
            
            ^ assembler'normalState.
        ]
        ! [ feedback raiseError:"Invalid token ":token. ].
].


#symbol indirectRegisterState = (:token:state:lineNumber:output)
[
    (state == dfaIdentifier)
        ? [
            #var ch1 := token@0 literal toLower &locale:currentLocale.
            #var num := control try:[ token literal Substring &at:1 toInt ] &catching:e [ feedback raiseError:"Invalid register name". ].
    
            (((ch1 == "r") || (ch1 == "s") || (ch1 == "d")) && (num < 15))
                ! [ feedback raiseError:"Register ":token:" is not valid". ].
                            
            (ch1 == "s") 
                ? [ num := num + 16. ].
            (ch1 == "d")
                ? [ num := num + 32. ].
                
            #var token := assembler'Tokens'TokenIndirectRegister new &register:num &line:lineNumber.
            
            output += token.
            
            ^ Wrap(token, assembler'indirectRegisterState2).
        ]
        ! [ feedback raiseError:"Invalid token ":token. ].
].

#symbol indirectRegisterState2 = (:token:state:lineNumber:output)
[
    (token == ":")
        ? [
            ^ Wrap(self token, assembler'offsetState).
        ]
        ! [ ^ assembler'normalState eval:token:state:lineNumber:output. ].
].

#symbol offsetState = (:token:state:lineNumber:output)
[
    (state == dfaDigit)
        ? [
            #var value := control try:
            [
                (token startingWith:"0x")
                    ? [ ^ token Substring &at:1 toHexLong. ]
                    ! [ ^ token toLong. ].
            ]
            &catching: e [ feedback raiseError:"Invalid numeric constant". ].

            self token set &offset:value.
            
            ^ assembler'normalState.
            
        ]
        ! [ feedback raiseError:"Invalid token ":token. ].    
].

#symbol normalState = (:token:state:lineNumber:output) 
[
    token =>
        "," ? [
            output += assembler'Tokens'TokenComma new &line_number:(lineNumber int).
            
            ^ assembler'normalState.
        ]
        "%" ? [
            ^ assembler'registerState.
        ]
        "@" ? [
            ^ assembler'indirectRegisterState.

        ]
        ! [
            state =>
                dfaEOF
                    ? [
                        output += TokenItem new &token:(assembler'Tokens'TokenEOL new &line_number:(lineNumber + 1)).
                        
                        ^ $nil.
                    ]
                dfaIdentifier 
                    ? [
                        output += assembler'Tokens'TokenIdentifier new &value:token &line_number:(lineNumber int).
                        
                        ^ assembler'normalState.
                    ]
                dfaDigit
                    ? [
                        #var value := control try:
                        [
                            (token startingWith:"0x")
                                ? [ ^ token Substring &at:1 toHexLong. ]
                                ! [ ^ token toLong. ].
                        ]
                        &catching: e [ feedback raiseError:"Invalid numeric constant". ].
                        
                        output += assembler'Tokens'TokenIntLiteral new &value:value &line_number:(lineNumber int).
                        
                        ^ assembler'normalState.
                    ]
                dfaLiteral
                    ? [
                        output += assembler'Tokens'TokenStringLiteral new 
                            &value:(token Substring:(token length - 2) &at:1) &line_number:(lineNumber int).
                            
                        ^ assembler'normalState.                            
                    ].
                
            feedback raiseError:"Invalid token ":token.            
        ].    
].

#class Lexer
{
    #field content.
    #field tokenList.
    
    #method TokenList = tokenList.
    
    #constructor new &sourceFile:aSource
    [
        content := File new &path:aSource literalContent.
        tokenList := List new.
    ]
    
    #method scan
    [
        #var lineNumber := Integer new:1.
        #var dfa := LexerDFA new.
        #var state := Variable new:normalState.
        content run &token_dfa:dfa &each:aToken
        [
            state << state eval:aToken:(dfa state):lineNumber:tokenList.
            
        ].
        
        // add virtual EOL
        state eval:"":dfaEOF:lineNumber:tokenList.
    ]
}