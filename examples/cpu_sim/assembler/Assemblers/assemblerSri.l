#import system.
#import system'collections.

#symbol AssemblerSri =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "rshi".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var operands := aIns Operands.
//        (operands length != 3)
//            ? [ console writeLine:(aIns Name + " does not take " + aIns Operands length literal). ^$nil. ].
//            
//        ((((operands@0 tokenType == assembler'TokenType Register) or: 
//           (operands@1 tokenType == assembler'TokenType Register) != true) or: 
//           (operands@2 tokenType == assembler'TokenType IntLiteral) != true))
//           ? [ console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'"). ^$nil. ].
//           
//        #var reg1 := (operands@0 token) Register.
//        #var reg2 := (operands@1 token) Register.
//        #var lite := (operands@2 token) Value.
//        #var opCode := assembler'Opcode Shr.
//        #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:reg1 &cpuReg2:reg2 &imm:lite.
//        aStr emit &data:tmp.
    ]
//
//    #method Mnemonic = mnemonic.
}.
