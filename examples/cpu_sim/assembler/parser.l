#import system.
#import system'math.
#import system'routines.
#import system'collections.
#import extensions.

#class Parser
{
    #field erros.
    #field instructions.
    #field tokens.
    #field position.
    
    #constructor new
    [
        erros := List new.
        instructions := List new.
        tokens := List new.
        position := IntNumber new &int:0.
    ]
    
    #method processTokens &tokens:aTokens
    [
        position := 0.
        tokens := aTokens.
        
        #loop (position < tokens length) ?
        [
            //#var tokenType := $self peekToken tokenType.
            //#var token := $self peekToken token.
            
            #var tmp := true.
            #loop ((position < tokens length) and: tmp)?
            [
                (($self peekToken) tokenType == assembler'TokenType EOL)
                ? [ $self readToken. ]
                ! [ tmp := false. ]. 
            ].
            
            (position < tokens length)?
            [
                #var tk1 := $self peekToken:1.
                #var tk2 := $self peekToken:0.
                ((tk1 tokenType == assembler'TokenType Colon) and: (tk2 tokenType == assembler'TokenType Identifier))
                    ? [ $self parseLabel. ]
                    ! [ $self parseSingleInstruction. ].
            ].
        ].
        
        #var globalScope := "".
        #var newInstructions := List new.
        #var i := 0.
        #loop (i < instructions length)?
        [
            #var o := instructions@i.

            (o Type) =>
                (assembler'TypeInstruction Instruction)
                ? [
                    (o Operands) run &each: token
                    [
                        (token tokenType == assembler'TokenType Identifier)?
                        [
                            ((token token Value) startingWith:".")
                                ? [ token token Value:(globalScope + token token Value). ].
                        ].
                    ].
                  ]
                  
                (assembler'TypeInstruction Label)
                ? [
                    ((o Name) startingWith:".")
                        ? [ o Name:(globalScope + o Name). ]
                        ! [ globalScope := o Name literal. ]. 
                  ].
                  
            newInstructions += o.
            i := i + 1.
        ].
        instructions clear.
        instructions := newInstructions clone. 
    ]
    
    #method parseLabel
    [
        #var tk := $self readToken token.
        $self readToken.
        instructions += (assembler'Label new:(tk Value)).
    ]
    
    #method parseSingleInstruction
    [
        #var list := List new.
        #loop ((position < tokens length) and: (($self peekToken) tokenType != assembler'TokenType EOL))
            ? [ list += ($self readToken). ].
        (list length == 0) ? [ ^$nil. ]. // do nothing... ??!!
        (list@0 tokenType == assembler'TokenType Identifier)
           ?[
                #var name := list@0 token literal.
                #var operands := List new.
                #var i := IntNumber new &int:1.
                
                #var skip := Boolean.
                #loop (i < list length)?
                [
                    skip := false
                    (i mod:2 != 0)? [ operands += list@i. skip := true.].
                    ((skip == false) and: (list@i tokenType != assembler'TokenType Comma))
                        ? [ console writeLine:"Comma or EOL expected!". ].
                    i := i + 1.
                ].
                instructions += (assembler'Instruction new &line:(list@0 token Line) &name:name &operands:operands).
            ]
           ![ console writeLine:"Mnemonic expected!". ]. 
    ]
    
    #method readToken
    [
        #var tk := tokens@position.
        position := position + 1.
        ^tk.
    ]
    
    #method peekToken
    [
        ^tokens@position.
    ]
    
    #method peekToken : pos
    [
        ((position + pos) < tokens length)
            ? [ ^tokens@(position + pos). ]
            ! [ ^(assembler'Tokens'TokenEOL new &line:0). ].            
    ]
    
    #method Output = instructions.
}