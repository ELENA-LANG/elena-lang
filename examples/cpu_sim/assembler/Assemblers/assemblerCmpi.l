#import system.
#import system'collections.

#symbol AssemblerCmpi =
{
    #method assemble &codeGen:codeGen &instruction:ins &byteCodeStream:output
    [
        #var operands := ins Operands.
        (operands length != 2)
            ? [ assembler'feedback raiseError:(ins Name):" does not take + ":(operands length). ].
 
        #var op1 := operands@0.
        #var op2 := operands@1.
        
        (op1 is &register && op2 is &numerical)
            ! [ assembler'feedback raiseError:"Unsupported addressing mode for instruction '":(ins Name):"'" ].

        #var reg1 := op1 Value.
        #var value := op2 Value.
        output emit &data:(assembler'CpuInstruction new &opcode:(assembler'Opcode Cmp) &cpuReg1:reg1 &imm:value).
    ]

    #method Mnemonic = "cmpi".
}.
