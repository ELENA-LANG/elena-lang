#import system.

#class InstructionXori
{
	#constructor new []

	#method interpret &ins:aInstruction &cpuvm:aVm
    [
        #var(type:bytearray,size:8)dump.
        #var(type:byte)register1.
        #var(type:byte)destReg.
        #var(type:int)imediate.
        
        destReg      := ((aInstruction shift &index:08) and: 00Fh).
        register1    := ((aInstruction shift &index:12) and: 00Fh).
        imediate     := ((aInstruction shift &index:16) and: 0FFFFh).
        
        #var op1 := aVm getRegister:destReg.
        #var result := op1 xor: imediate.
        
        aVm setRegister &register:destReg &value:result.
    ]
}
