#import system.

#class InstructionSw
{
	#constructor new []

	#method interpret &ins:aInstruction &cpuvm:aVm
    [
        #var(type:byte)destinationRegister.
        #var(type:byte)sourceRegister.
        #var(type:short)destinationOffset.
        
        destinationRegister := ((aInstruction shift &index:08) and: 00Fh).
        sourceRegister      := ((aInstruction shift &index:12) and: 00Fh).
        destinationOffset   := ((aInstruction shift &index:16) and: 0FFFFh).
        
        #var address := (aVm getRegister:destinationRegister) + destinationOffset.
        (aVm getMemory) writeWord &address:address &byt:((aVm getRegister:sourceRegister)).
    ]
}
