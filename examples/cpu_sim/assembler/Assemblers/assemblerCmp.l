#import system.
#import system'collections.

#symbol AssemblerCmp =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "cmp".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var operands := aIns Operands.
//        (operands length != 2)
//            ? [ console writeLine:(aIns Name + " does not take + " + aIns Operands length literal). ^$nil. ].
//            
//        (((operands@0 tokenType == assembler'TokenType Register) or: 
//          (operands@1 tokenType == assembler'TokenType Register) != true))
//           ? [ console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'"). ^$nil. ].
//           
//        #var reg1 := (operands@0 token) Register.
//        #var reg2 := (operands@1 token) Register.
//        #var opCode := assembler'Opcode Cmp.
//        #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:reg1 &cpuReg2:reg2.
//        aStr emit &data:tmp.
    ]
//
//    #method Mnemonic = mnemonic.
}.
