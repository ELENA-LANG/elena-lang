#import system.
#import system'collections.

#symbol AssemblerLi =
{
    #method assemble &codeGen:aCodeGen &instruction:ins &byteCodeStream:output
    [
        #var operands := ins Operands.
        (operands length != 2)
            ? [ assembler'feedback raiseError:(ins Name):" does not take + ":(operands length). ].
        
        #var op1 := operands@0.
        #var op2 := operands@1.
            
        ((op1 is &register) && (op2 is &numerical)) ?
        [
            #var register := op1 Value.
            #var val := op2 Value.            
            output emit &data:(assembler'CpuInstruction new &opcode:(assembler'Opcode Li) &cpuReg1:register &imm:val).
        ].

        ((op1 is &register) && (op2 is &identifier)) ?
        [
            #var register := op1 Value.
            #var identifier := op2 Value.
            output emit &data:(assembler'CpuInstruction new &opcode:(assembler'Opcode Li) &cpuReg1:register &strImm:identifier).
        ].
    ]

    #method Mnemonic = "li".
}.