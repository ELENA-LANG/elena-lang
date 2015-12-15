#import system.
#import system'collections.

#symbol AssemblerCmpi =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "cmpi".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var operands := aIns Operands.
//        (operands length != 2)
//            ? [ console writeLine:(aIns Name + " does not take " + aIns Operands length literal). ^$nil. ].
//            
//        (((operands@0 tokenType == assembler'TokenType Register) or: 
//          (operands@1 tokenType == assembler'TokenType IntLiteral) != true))
//           ? [ console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'"). ^$nil. ].
//           
//        #var reg1  := (operands@0 token) Register.
//        #var value := (operands@1 token) Value.
//        #var opCode := assembler'Opcode Cmp.
//        #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:reg1 &imm:value.
//        aStr emit &data:tmp.
    ]
//
//    #method Mnemonic = mnemonic.
}.
