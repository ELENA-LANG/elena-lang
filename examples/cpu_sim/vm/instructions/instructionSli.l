#import system.

#class InstructionSli
{
	#constructor new []

	#method interpret &ins:aInstruction &cpuvm:aVm
    [
        #var(type:byte)register1.
        #var(type:short)immediate.
        #var(type:byte)destRegister.
        
        destRegister := ((aInstruction shift &index:08) and: 00Fh).
        register1    := ((aInstruction shift &index:12) and: 00Fh).
        immediate    := ((aInstruction shift &index:16) and: 0FFFFh).
        
        #var op1 := aVm getRegister:register1.
        #var result := op1 shift &index:(immediate * -1).
        aVm setRegister &register:destRegister &value:result.
    ]
}
