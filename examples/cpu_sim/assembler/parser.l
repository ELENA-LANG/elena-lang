#import system.
#import system'math.
#import system'routines.
#import system'collections.
#import extensions.

#class Parser
{
    //#field erros.
    #field theInstructions.
    
    #constructor new
    [
        theInstructions := List new.
        //erros := List new.
    ]
    
    #method processTokens &tokens:tokens
    [
        #var it := tokens iterator.
        
        #loop (it available)?
        [
//                #var tk1 := $self peekToken:1.
//                #var tk2 := $self peekToken:0.

/*                ((tk1 tokenType == assembler'TokenType Colon) and: (tk2 tokenType == assembler'TokenType Identifier))
                    ? [ $self parseLabel. ]
                    ! [ */$self parseSingleInstruction:it. /*].*/
            
        ].
        
        
//        #var globalScope := "".
//        #var newInstructions := List new.
//        #var i := 0.
//        #loop (i < instructions length)?
//        [
//            #var o := instructions@i.
//
//            (o Type) =>
//                (assembler'TypeInstruction Instruction)
//                ? [
//                    (o Operands) run &each: token
//                    [
//                        (token tokenType == assembler'TokenType Identifier)?
//                        [
//                            ((token token Value) startingWith:".")
//                                ? [ token token Value:(globalScope + token token Value). ].
//                        ].
//                    ].
//                  ]
//                  
//                (assembler'TypeInstruction Label)
//                ? [
//                    ((o Name) startingWith:".")
//                        ? [ o Name:(globalScope + o Name). ]
//                        ! [ globalScope := o Name literal. ]. 
//                  ].
//                  
//            newInstructions += o.
//            i := i + 1.
//        ].
//        instructions clear.
//        instructions := newInstructions clone. 
    ]
    
//    #method parseLabel
//    [
//        #var tk := $self readToken token.
//        $self readToken.
//        instructions += (assembler'Label new:(tk Value)).
//    ]
    
    #method parseOperands : it
    [
        #var list := List new.
        #var(type:bool) operandExpected := false.
        #var(type:bool) commaExpected := false.
        #loop (it available and:[ it get is &eol not ])?
        [
            (it get is &comma)
                ? [
                    commaExpected
                        ! [ feedback raiseError:"operand expected!". ].
                        
                    operandExpected := true.
                    commaExpected := false.
                ]
                ! [
                    commaExpected
                        ? [ feedback raiseError:"operand expected!". ].
                        
                    list += it get.
                    
                    operandExpected := false.
                    commaExpected := true.
                ].
                        
            it append &index:1.
        ].
        
        operandExpected
            ? [ feedback raiseError:"operand expected!". ].
        
        ^ list toArray.
    ]
    
    #method parseSingleInstruction : it
    [
        (it get is &identifier)
            ? [                
                #var opcode := it get Value.
                #var lineNumber := it get line_number.
                #var operands := $self parseOperands:(it append &index:1).
                
                theInstructions += assembler'Instruction new &name:opcode &operands:operands &line_number:lineNumber.
                
                it append &index:1.
            ]
            ! [ feedback raiseError:"Mnemonic expected!". ].
    ]
    
    #method Output = theInstructions.
}