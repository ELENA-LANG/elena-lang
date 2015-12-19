#import system.
#import system'collections.

#symbol AssemblerSw =
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

        #var reg1 := op1 Value.
        #var reg2 := op2 Value.
        #var offset := op2 offset.
        
        output emit &data:(assembler'CpuInstruction new &opcode:(assembler'Opcode Sw) &cpuReg1:reg1 &cpuReg2:reg2 &imm:offset).
    ]
    
    #method Mnemonic = "sw".
}.
