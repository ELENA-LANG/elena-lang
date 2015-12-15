#import system.
#import system'collections.

#symbol AssemblerBr =
{
    #method assemble &codeGen:codeGen &instruction:ins &byteCodeStream:output
    [
        #var operands := ins Operands.
        (operands length != 1)
            ? [ assembler'feedback raiseError:(ins Name):" does not take + ":(operands length). ].
 
        #var op := operands@0.

        (op is &identifier)
            ! [ assembler'feedback raiseError:"Unsupported addressing mode for instruction '":(ins Name):"'" ].
                       
        #var conditionCode := assembler'ConditionCode Always.            
        (ins Name) =>
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
                
        output emit &data:(assembler'CpuInstruction new &opcode:(assembler'Opcode Br) &conditionCode:conditionCode &sym:(op Value) &relative:true).
    ]

    #method Mnemonic = ("br", "bs", "bv", "bc", "bz", "be", "bne", "ba", "bb", "blt", "bgt", "ble", "bge").
}.
