#import system.
#import system'collections.

#class AssemblerLi
{
    #field mnemonic.
    
    #constructor new
    [
        mnemonic := List new.
        mnemonic += "li".
    ]
    
    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
        #var operands := aIns Operands.
        (operands length != 2)
            ? [ console writeLine:(aIns Name + " does not take + " + aIns Operands length literal). ].
        
        ((operands@0 tokenType == assembler'TokenType Register) && (operands@1 tokenType == assembler'TokenType IntLiteral))?
        [
            #var register := (operands@0 token) Register.
            #var val := (operands@1 token) Value.
            #var opCode := assembler'Opcode Li.
            #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:register &imm:val.
            aStr emit &data:tmp.
        ].
        
        ((operands@0 tokenType == assembler'TokenType Register) && (operands@1 tokenType == assembler'TokenType Identifier))?
        [
            #var register := (operands@0 token) Register.
            #var identifier := (operands@1 token) Value.
            #var opCode := assembler'Opcode Li.
            #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:register &strImm:identifier.
            aStr emit &data:tmp.
        ].
    ]
    
    #method Mnemonic = mnemonic.
}