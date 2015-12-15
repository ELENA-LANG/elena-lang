#import system.
#import system'collections.

#symbol AssemblerRet =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "rtn".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var operands := aIns Operands.
//        (operands length != 0)
//            ? [ console writeLine:(aIns Name + " does not take + " + aIns Operands length literal). ]
//            ! [
//                #var reg15 := assembler'Registers R15.
//                #var reg11 := assembler'Registers R11.
//                #var opCode := assembler'Opcode Add.
//                #var tmp := assembler'CpuInstruction new &opcode:opCode &cpuReg1:reg15 &cpuReg2:reg11 &imm:0.
//                aStr emit &data:tmp.
//              ].
    ]
//
//    #method Mnemonic = mnemonic.
}.
