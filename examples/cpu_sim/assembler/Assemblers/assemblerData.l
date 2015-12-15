#import system.
#import system'routines.
#import system'collections.

#symbol AssemblerData =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "data".
//        mnemonic += ".data".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var listTmp := List new.
//        (aIns Operands) run &each: op
//        [
//            (op tokenType) =>
//                (assembler'TokenType StringLiteral)?
//                [
//                    #var value := op token Value.
//                    // Get the bytes from token Value
//                    value run &each: aChar [ listTmp += ((aChar int) byte). ].
//                ]
//                (assembler'TokenType IntLiteral)?
//                [
//                    listTmp += (((op token Value) int) byte).
//                ].
//        ].
//        #var tmp := assembler'CpuData new &data:listTmp.
//        aStr emit &data:tmp.
    ]
//
//    #method Mnemonic = mnemonic.
}.
