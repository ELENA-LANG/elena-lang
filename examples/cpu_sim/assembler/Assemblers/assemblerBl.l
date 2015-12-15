#import system.
#import system'collections.

#symbol AssemblerBl =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "bl".
//        mnemonic += "bls".
//        mnemonic += "blv".
//        mnemonic += "blc".
//        mnemonic += "blz".
//        mnemonic += "ble".
//        mnemonic += "blne".
//        mnemonic += "bll".
//        mnemonic += "blg".
//        mnemonic += "blle".
//        mnemonic += "blge".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var operands := aIns Operands.
//        (operands length != 1)
//            ? [ console writeLine:(aIns Name + " does not take + " + aIns Operands length literal). ^$nil. ].
//            
//        (operands@0 tokenType == assembler'TokenType Identifier)?
//        [
//            #var lbl := operands@0 token.
//            #var conditionCode := assembler'ConditionCode Always.
//            
//            (aIns Name) =>
//                "blc" ?  [ conditionCode := assembler'ConditionCode Carry. ]
//                "bls" ?  [ conditionCode := assembler'ConditionCode Sign. ]
//                "blv" ?  [ conditionCode := assembler'ConditionCode Overflow. ]
//                "blz" ?  [ conditionCode := assembler'ConditionCode Zero. ]
//                "ble" ?  [ conditionCode := assembler'ConditionCode Equal. ]
//                "blne" ? [ conditionCode := assembler'ConditionCode NotEqual. ]
//                "bll" ?  [ conditionCode := assembler'ConditionCode LessThan. ]
//                "blg" ?  [ conditionCode := assembler'ConditionCode GreaterThan. ]
//                "blle" ? [ conditionCode := assembler'ConditionCode LessThanOrEqual. ]
//                "blge" ? [ conditionCode := assembler'ConditionCode GreaterThanOrEqual. ].
//                
//            #var opCode := assembler'Opcode Bl.
//            #var tmp := assembler'CpuInstruction new &opcode:opCode &conditionCode:conditionCode &sym:(lbl Value) &relative:true.
//            aStr emit &data:tmp.
//        ].
    ]

//    #method Mnemonic = mnemonic.
}.
