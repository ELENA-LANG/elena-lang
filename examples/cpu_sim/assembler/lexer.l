#import system.
#import system'routines.
#import system'collections.
#import system'io.
#import extensions.
#import extensions'routines.
#import extensions'text.
#import system'culture.

#symbol(const,type:char) LF  = #0Ah.
#symbol(const,type:char) CR  = #0Dh.
#symbol(const,type:char) TAB = #09h.
#symbol(const,type:char) SPACE = #20h.

#symbol $isWhiteSpace = ch
[
    #var c := ch /*toInt*/.
    ($nil == c)
        ? [ ^false. ]
        ! [ ^((c == SPACE) or: (c == TAB) or: (c == LF) or: (c == CR)). ].
].

#symbol $isDigit = ch
[
    #var c := ch /*toInt*/.
    ($nil == c)
        ? [ ^false. ]
        ! [ ^((c >= #30h) and: (c <= #39h)). ].
].

#symbol $isLetter = ch
[
    #var c := ch /*toInt*/.
    ($nil == c)
        ? [ ^false. ]
        ! [ ^(((c >= #41h) and: (c <= #5Ah)) or: ((c >= #61h) and: (c <= #7Ah))). ].
].

#symbol $isLetterOrDigit = ch
[
    #var c := ch /*toInt*/.
    ($nil == c)
        ? [ ^false. ]
        ! [ ^(($isLetter:c) or: ($isDigit:c) or: (c == #5Fh)). ].
].

#symbol $htoi = hexString
[
    #var i := 0.
    #var(type:int) ret := 0.
    #var tmp := 0.
    
    #loop (i < hexString length) ?
    [
        #var c := hexString@i.
        ((c >= #65) and: (c <= #70))
            ? [ tmp := (c toInt) - 65 + 10. ].              
        ((c >= #97) and: (c <= #102)) 
            ? [ tmp := (c toInt) - 97 + 10. ].
        ($isDigit:c)
            ? [ tmp := (c toInt) - 48. ].
            
        ret := (16 * ret) + tmp.
        i := i + 1.
    ].
    ^ret.
].

#symbol $converToInt = str
[
    #var i := 0.
    #var ret := 0.
    #loop (i<str length)?
    [
        ((str@i != SPACE) && ($isDigit:(str@i)))?
        [
            ret := ret * 10 + ((str@i) int) - 48.
        ].
        i := i + 1.
    ].
    ^ret.
].

#class LexerDFA :: Statemachine
{
    #field(type:bool)theSpaceMode.
    #field(type:bool)theSingleTokenMode.
    
    #constructor new
    [
        theSpaceMode := true.
        theSingleTokenMode := false.
    ]
    
    #method proceed &char:aChar
    [
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
                    ? [  theSingleTokenMode := false. theSpaceMode := false. ^ true. ].
                
                (letter or:digit)
                    ! [ 
                        theSpaceMode
                            ! [ theSpaceMode := true. ^ true. ].
                            
                        theSingleTokenMode := true.                         
                    ].
                    
                theSpaceMode := false.                    
            ].
            
        ^ false.            
    ]
    
    #method is &whitespace = theSpaceMode.    
}

#class Lexer
{
    #field content.
    
    
    #field lineNumber.
    #field position.
    #field tokenList.
    
    #method TokenList = tokenList.
    
    #constructor new &sourceFile:aSource
    [
        content := File new &path:aSource literalContent.
        
        
        
        position := 0.
        lineNumber := 0.
        tokenList := List new.        
    ]
    
    #method scan
    [
        content run &token_dfa:(LexerDFA new) &each:aToken
        [
            //console writeLine:aToken.
        ].
        
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
//    #method scanIntLiteral
//    [
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
//    ]
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
//    #method scanRegister
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
////        #var register := (regNumber /*trimRight*/) toInt.   
//        #var register := $converToInt:regNumber.
//        #var tmpReg := (accum@0 literal) toLower &locale:currentLocale.
//        (((tmpReg != "r") or: (tmpReg != "s") or: (tmpReg != "d") == false) or: (register > 15))
//            ? [
//                console writeLine:("Register, " + accum + ", not valid, in line: " + ((lineNumber + 1) literal)). 
//              ].
//                   
//        (tmpReg == "s") 
//            ? [ register := register + 16. ].
//        (tmpReg == "d")
//            ? [ register := register + 32. ].
//            
//        ^(assembler'Tokens'TokenRegister new &register:register &line:lineNumber).
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