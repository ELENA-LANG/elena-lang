#import system.
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
    
    #method parseIntToken : token : lineNumber
    [
        #var value := 0l.
        (token startingWith:"0x")
            ? [
                value := 
            ]
            ! [
                value := control try:[ token toHexLong ] &catching: e [ feedback raiseError:"Invalid numeric constant". ].
            ].
        
        assembler'Tokens'TokenIntLiteral new &value:value &line_number:(lineNumber int).
        
//        #var v1 := $self peekChar:0.
//        #var v2 := $self peekChar:1.
//        
//        ((v1 == #30h) and: (v2 == #78h))
//            ? [ 
//            ^($self scanHexNumber).
//             ]
//            ! [
//                #var accum := "".
//                accum := accum + ($self readChar) literal.
//                #loop ($isDigit:($self peekChar))?
//                [
//                    accum := accum + ($self readChar) literal.
//                ].
//                ^(assembler'Tokens'TokenIntLiteral new &value:(accum toLong) &line:lineNumber).
//              ].
    ]

    #method parseNormalToken : dfa : token : lineNumber
    [
        (dfa state) =>
            dfaIdentifier 
                ? [
                    ^ assembler'Tokens'TokenIdentifier new &value:token &line_number:(lineNumber int).
                ]
            dfaDigit
                ? [
                    ^ $self parseIntToken:token:lineNumber.
                ]
            dfaLiteral
                ? [
                    ^ assembler'Tokens'TokenStringLiteral new 
                        &value:(token Substring:(token length - 2) &at:1) &line_number:(lineNumber int).
                ]
            ! [
                token =>
                    "," ? [ ^ assembler'Tokens'TokenComma new &line_number:(lineNumber int). ].
            ].
            
        feedback raiseError:"Invalid token ":token.            
    ]
    
    #method parseRegister : token : lineNumber
    [
        #var ch1 := token@0 literal toLower &locale:currentLocale.
        #var num := control try:[ token literal Substring &at:1 toInt ] &catching:e [ feedback raiseError:"Invalid register name". ].

        (((ch1 == "r") || (ch1 == "s") || (ch1 == "d")) && (num < 15))
            ! [ feedback raiseError:"Register ":token:" is not valid". ].
                        
        (ch1 == "s") 
            ? [ num := num + 16. ].
        (ch1 == "d")
            ? [ num := num + 32. ].
            
        ^ assembler'Tokens'TokenRegister new &register:num &line:lineNumber.
    ]
    
    #method scan
    [
        #var lineNumber := Integer new:1.
        #var dfa := LexerDFA new.
        #var registerExpected := Boolean new:false.
        #var indirectRegisterExpected := Boolean new:false.
        content run &token_dfa:dfa &each:aToken
        [
            aToken =>
                "%" ? [
                    (registerExpected || indirectRegisterExpected)
                        ? [ feedback raiseError:"Register expected". ]
                        ! [ registerExpected << true. ].
                ]
                "@" ? [
                    (registerExpected || indirectRegisterExpected)
                        ? [ feedback raiseError:"Inirect Register expected". ]
                        ! [ indirectRegisterExpected << true. ].
                ]
                ! [
                    #var tokenAttr := registerExpected iif:($self parseRegister:aToken:lineNumber)
                                        :(indirectRegisterExpected iif:($self parseIndirectRegister:dfa:aToken:lineNumber)
                                            :($self parseNormalToken:dfa:aToken:lineNumber)).
                    registerExpected << false.
                    indirectRegisterExpected << false.
                    
                    tokenList += TokenItem new &token:tokenAttr.
                ].
        ].
        
        // add virtual EOL
        tokenList += TokenItem new &token:(assembler'Tokens'TokenEOL new &line_number:(lineNumber + 1)).
        
//        #var skipThis := false.
//        #loop (position < sourceFile length)?
//        [           
//            #loop (($self peekChar != LF) and: ($isWhiteSpace:($self peekChar)))
//                ? [ $self readChar. ].
//            
//            skipThis := false. 
//                
//            (($self peekChar) == LF) ?
//            [
//                #var brk := false.
//                #loop (($nil != ($self peekChar)) and: (brk != true))?
//                [
//                    (($self peekChar) == LF)? [ brk := true. ].
//                    $self readChar.
//                    lineNumber := lineNumber + 1.
//                ].
//                skipThis := true.
//                
//                #var auxTk := assembler'Tokens'TokenEOL new &line:lineNumber.
//                tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType EOL)).
//                //tokenList += (assembler'Tokens'TokenEOL new &line:lineNumber).
//            ].
//                   
//            ((skipThis == false) and: ($isDigit:($self peekChar)))?
//            [
//                #var auxTk := $self scanIntLiteral.
//                tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType IntLiteral)).
//                //tokenList += ($self scanIntLiteral).
//                skipThis := true.
//            ].
//            
//            ((skipThis == false) and: ($nil != $self peekChar)) ? // 0x22 == '\"'
//            [
//                (($self peekChar) == #22h)?
//                [
//                    #var auxTk := $self scanStringLiteral.
//                    tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType StringLiteral)).
//                    //tokenList += ($self scanStringLiteral).
//                    skipThis := true.
//                ].
//            ].
//            
//            ((skipThis == false) and: ($nil != $self peekChar)) ? // 0x3B == ';'
//            [
//                (($self peekChar) == #3Bh)?
//                [
//                    #loop (($self peekChar toInt) != LF) ? [ $self readChar. ]. 
//                    skipThis := true.
//                ].
//            ].
//            
//            ((skipThis == false) and: ($nil != $self peekChar)) ? // 0x2E == '.'
//            [
//                (($isLetter:($self peekChar)) or: (($self peekChar) == #2Eh))?
//                [
//                    #var auxTk := $self scanIdentifier.
//                    tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType Identifier)).
//                    //tokenList += ($self scanIdentifier).
//                    skipThis := true.
//                ].
//            ].
//            
//            (skipThis == false) ?
//            [
//                ($self peekChar) =>
//                    #25h ? [ // '%'
//                            #var auxTk := $self scanRegister.
//                            tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType Register)).
//                            //tokenList += ($self scanRegister). 
//                          ]
//                    #40h ? [ // '@'
//                            #var auxTk := $self scanIndirectRegister.
//                            tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType IndirectRegister)).
//                            //tokenList += ($self scanIndirectRegister). 
//                          ]
//                    #2Ch ? [ // ',' 
//                            $self readChar. 
//                            #var auxTk := assembler'Tokens'TokenComma new &line:lineNumber.
//                            tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType Comma)).
//                            //tokenList += (assembler'Tokens'TokenComma new &line:lineNumber). 
//                          ]
//                    #23h ? [ // '#'
//                            $self readChar. 
//                            #var auxTk := assembler'Tokens'TokenHash new &line:lineNumber.
//                            tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType Hash)).
//                            //tokenList += (assembler'Tokens'TokenHash new &line:lineNumber).
//                          ]
//                    #3Ah ? [ // ':'
//                            $self readChar.
//                            #var auxTk := assembler'Tokens'TokenColon new &line:lineNumber.
//                            tokenList += (assembler'Token new &token:auxTk &tokenType:(assembler'TokenType Colon)).
//                            //tokenList += (assembler'Tokens'TokenColon new &line:lineNumber). 
//                          ]
//                          ! [ 
//                          console writeLine:"aqui". ].                              
//            ].
//        ].
    ]
    
//    #method peekChar
//    [
//        ^($self peekChar:0).
//    ]
//    
//    #method peekChar : pos
//    [
//        #var ret := $nil.
//        ((pos + position) < sourceFile length)
//           ? [ ret := sourceFile@(pos + position) /*toInt*/. ].
//        ^ret.
//    ]
//    
//    #method readChar
//    [
//        #var ret := $nil.
//        (position < sourceFile length)
//            ? [ ret := sourceFile@(position). position := position + 1. ].
//        ^ret.        
//    ]
    
    
//    // --- Assembler analises...
//    
//    #method scanHexNumber
//    [
//        // skip the 0x
//        $self readChar.
//        $self readChar.
//        
//        #var accum := "".
//        #var hexAlpha := "abcdefABCDEF".
//        accum := accum + ($self readChar) literal. 
//        #loop (($isDigit:($self peekChar)) or: (hexAlpha containing:($self peekChar)))?
//        [
//            accum := accum + ($self readChar) literal.
//        ].
//
//        #var tmp := $htoi:accum.
//        ^(assembler'Tokens'TokenIntLiteral new &value:(tmp toLong) &line:lineNumber).
//    ]
//    
//    #method scanStringLiteral
//    [
//        $self readChar.
//        #var accum := "".
//        #loop (($self peekChar toInt != 022h) and: ($nil != $self peekChar)) ?
//        [
//            accum := accum + $self readChar.
//        ].
//        $self readChar.
//        ^(assembler'Tokens'TokenStringLiteral new &value:accum &line:lineNumber).
//    ]
//    
//    #method scanIdentifier
//    [
//        #var accum := "".
//        accum := accum + $self readChar.
//        
//        #loop ($isLetterOrDigit:($self peekChar))
//            ? [ accum := accum + $self readChar. ].
//        ^(assembler'Tokens'TokenIdentifier new &name:accum &line:lineNumber).
//    ]
//    
//    #method scanIndirectRegister
//    [
//        $self readChar.
//        #var accum := "".
//        
//        #var auxChar := $nil.
//        #loop (($isLetterOrDigit:($self peekChar)) and: ($nil != $self peekChar)) ?
//        [
//            accum := accum + $self readChar.
//        ].
//        
//        
//        #var(type:int) accumSize := accum length.
//        #var regNumber := accum Substring &int:1 &int:accumSize.
//        #var i := 1.
//        #var isDigit := true.
//        #loop ((i < regNumber length) and: isDigit) ?
//        [
//            isDigit := $isDigit:(regNumber@i).
//            i := i + 1.
//        ].
//        
//        #var register := (regNumber trimRight) toInt.        
//        #var tmpReg := (accum@0 literal) toLower &locale:currentLocale.
//        (((tmpReg != "r") or: (tmpReg != "s") or: (tmpReg != "d") == false) or: (register > 15))
//            ? [
//                console writeLine:("Register, " + accum + ", not valid, in line: " + ((lineNumber + 1) literal)). 
//              ].
//              
//        ($self peekChar literal == ":")
//        ? [
//            $self readChar.
//            #var il := $self scanIntLiteral.
//            ^(assembler'Tokens'TokenIndirectRegister new &register:register &offset:(il getValue) &line:lineNumber).
//          ]
//        ! [ ^(assembler'Tokens'TokenIndirectRegister new &register:register &line:lineNumber). ].
//          
//    ]
}