#import system.
#import system'collections.

#symbol AssemblerAddi =
{
    #method assemble &codeGen:gen &instruction:ins &byteCodeStream:output
    [        
        #var operands := ins Operands.
        (operands length != 3)
            ? [ assembler'feedback raiseError:(ins Name):" does not take + ":(operands length). ].
 
        #var op1 := operands@0.
        #var op2 := operands@1.
        #var op3 := operands@2.

        (op1 is &register && op2 is &register && op3 is &numerical)
            ! [ assembler'feedback raiseError:"Unsupported addressing mode for instruction '":(ins Name):"'" ].

        #var reg1 := op1 Value.
        #var reg2 := op2 Value.
        #var lit := op3 Value.
        output emit &data:(assembler'CpuInstruction new &opcode:(assembler'Opcode Add) &cpuReg1:reg1 &cpuReg2:reg2 &imm:lit).
    ]

    #method Mnemonic = "addi".
}.
