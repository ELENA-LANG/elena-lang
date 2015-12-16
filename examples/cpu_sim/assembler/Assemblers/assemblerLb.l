#import system.

#symbol AssemblerLb =
{
    #method assemble &codeGen:codeGen &instruction:ins &byteCodeStream:output
    [
        #var operands := ins Operands.
        (operands length != 2)
            ? [ assembler'feedback raiseError:(ins Name):" does not take + ":(operands length). ].

        #var op1 := operands@0.
        #var op2 := operands@1.

        (op1 is &register && op2 is &indirect_register)
            ! [ assembler'feedback raiseError:"Unsupported addressing mode for instruction '":(ins Name):"'" ].

        #var dst := op1 Value.
        #var src := op2 Value.

        output emit &data:(assembler'CpuInstruction new &opcode:(assembler'Opcode Lb) &cpuReg1:dst &cpuReg2:src &imm:(op2 Offset)).
    ]

    #method Mnemonic = "lb".
}.
