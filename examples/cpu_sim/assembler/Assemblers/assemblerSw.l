#import system.
#import system'collections.

#symbol AssemblerSw =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "sw".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var operands := aIns Operands.
//        (operands length != 2)
//            ? [ console writeLine:(aIns Name + " does not take " + aIns Operands length literal). ^$nil. ].
//        (((operands@0 tokenType == assembler'TokenType Register) or: 
//          (operands@1 tokenType == assembler'TokenType IndirectRegister) != true))
//          ? [ console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'"). ^$nil. ].
//           
//        #var dest := (operands@0 token) Register.
//        #var reg  := (operands@1 token) Register.
//        #var offset := (operands@0 token) Value.
//        #var opCode := assembler'Opcode Sw.
//        #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:dest &cpuReg2:reg &imm:offset.
//        aStr emit &data:tmp.
    ]

//    #method Mnemonic = mnemonic.
}.
