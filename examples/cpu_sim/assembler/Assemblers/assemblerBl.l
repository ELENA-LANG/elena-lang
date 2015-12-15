#import system.
#import system'collections.

#symbol AssemblerBl =
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
            "blc" ?  [ conditionCode := assembler'ConditionCode Carry. ]
            "bls" ?  [ conditionCode := assembler'ConditionCode Sign. ]
            "blv" ?  [ conditionCode := assembler'ConditionCode Overflow. ]
            "blz" ?  [ conditionCode := assembler'ConditionCode Zero. ]
            "ble" ?  [ conditionCode := assembler'ConditionCode Equal. ]
            "blne" ? [ conditionCode := assembler'ConditionCode NotEqual. ]
            "bll" ?  [ conditionCode := assembler'ConditionCode LessThan. ]
            "blg" ?  [ conditionCode := assembler'ConditionCode GreaterThan. ]
            "blle" ? [ conditionCode := assembler'ConditionCode LessThanOrEqual. ]
            "blge" ? [ conditionCode := assembler'ConditionCode GreaterThanOrEqual. ].
                
        output emit &data:(assembler'CpuInstruction new &opcode:(assembler'Opcode Bl) &conditionCode:conditionCode &sym:(op Value) &relative:true).
    ]

    #method Mnemonic = ("bl","bls","blv","blc","blz","ble","blne","bll", "blg", "blle", "blge").
}.
