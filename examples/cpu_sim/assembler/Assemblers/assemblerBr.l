#import system.
#import system'collections.

#class AssemblerBr
{
    #field mnemonic.

    #constructor new
    [
        mnemonic := List new.
        mnemonic += "br".
        mnemonic += "bs".
        mnemonic += "bv".
        mnemonic += "bc".
        mnemonic += "bz".
        mnemonic += "be".
        mnemonic += "bne".
        mnemonic += "ba".
        mnemonic += "bb".
        mnemonic += "blt".
        mnemonic += "bgt".
        mnemonic += "ble".
        mnemonic += "bge".
    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
        #var operands := aIns Operands.
        (operands length != 1)
            ? [ console writeLine:(aIns Name + " does not take + " + aIns Operands length literal). ^$nil. ].
            
        (operands@0 tokenType == assembler'TokenType Identifier)?
        [
            #var lbl := operands@0 token.
            #var conditionCode := assembler'ConditionCode Always.
            
            (aIns Name) =>
                "bc" ?  [ conditionCode := assembler'ConditionCode Carry. ]
                "bs" ?  [ conditionCode := assembler'ConditionCode Sign. ]
                "bv" ?  [ conditionCode := assembler'ConditionCode Overflow. ]
                "bz" ?  [ conditionCode := assembler'ConditionCode Zero. ]
                "be" ?  [ conditionCode := assembler'ConditionCode Equal. ]
                "bne" ? [ conditionCode := assembler'ConditionCode NotEqual. ]
                "blt" ? [ conditionCode := assembler'ConditionCode LessThan. ]
                "bgt" ? [ conditionCode := assembler'ConditionCode GreaterThan. ]
                "ble" ? [ conditionCode := assembler'ConditionCode LessThanOrEqual. ]
                "bge" ? [ conditionCode := assembler'ConditionCode GreaterThanOrEqual. ].
                
            #var opCode := assembler'Opcode Br.
            #var tmp := assembler'CpuInstruction new &opcode:opCode &conditionCode:conditionCode &sym:(lbl Value) &relative:true.
            aStr emit &data:tmp.
        ].
    ]

    #method Mnemonic = mnemonic.
}
