#import system.
#import system'collections.

#class AssemblerSubi
{
    #field mnemonic.

    #constructor new
    [
        mnemonic := List new.
        mnemonic += "subi".
    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
        #var operands := aIns Operands.
        (operands length != 3)
            ? [ console writeLine:(aIns Name + " does not take + " + aIns Operands length literal). ].
            
        ((((operands@0 tokenType == assembler'TokenType Register) or: 
           (operands@1 tokenType == assembler'TokenType Register) != true) or: 
           (operands@2 tokenType == assembler'TokenType IntLiteral) != true))
           ? [ console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'"). ^$nil. ].
            
        #var reg1 := (operands@0 token) Register.
        #var reg2 := (operands@1 token) Register.
        #var lit  := (operands@2 token) Value.
        #var opCode := assembler'Opcode Sub.
        #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:reg1 &cpuReg2:reg2 &imm:lit.
        aStr emit &data:tmp.
    ]

    #method Mnemonic = mnemonic.
}
