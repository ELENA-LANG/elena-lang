#import system.
#import system'collections.

#class AssemblerLw
{
    #field mnemonic.

    #constructor new
    [
        mnemonic := List new.
        mnemonic += "lw".
    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
        #var operands := aIns Operands.
        (operands length != 2)
            ? [ console writeLine:(aIns Name + " does not take " + aIns Operands length literal). ^$nil. ].
            
        (((operands@0 tokenType == assembler'TokenType Register) or: 
          (operands@1 tokenType == assembler'TokenType IndirectRegister) != true))
          ? [ console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'"). ^$nil. ].
           
        #var dst := (operands@0 token) Register.
        #var src := (operands@1 token) Register.
        #var opCode := assembler'Opcode Lw.
        #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:dst &cpuReg2:src &imm:(operands@1 token Offset).
        aStr emit &data:tmp.
    ]

    #method Mnemonic = mnemonic.
}
