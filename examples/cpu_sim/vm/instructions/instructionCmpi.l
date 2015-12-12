#import system.

#class InstructionCmpi
{
	#constructor new []

	#method interpret &ins:aInstruction &cpuvm:aVm
    [
        #var(type:byte)register1.
        #var(type:short)op2.
        
        register1 := ((aInstruction shift &index:08) and: 00Fh).
        op2       := ((aInstruction shift &index:16) and: 0FFFFh).
        
        #var op1 := aVm getRegister:register1.
        #var flags := aVm getRegister:12.
        #var result := op1 - op2.
        
        flags := flags and: (cpuvm'Flags Carry not).
        flags := flags and: (cpuvm'Flags Overflow not).
        flags := flags and: (cpuvm'Flags Sign not).
        
        (result == 0)
            ? [ flags := flags or: (cpuvm'Flags Zero). ]
            ! [ flags := flags and: ((cpuvm'Flags Zero) not). ].
            
        (result < 0)
            ? [ flags := flags or: (cpuvm'Flags Sign). ]
            ! [ flags := flags and: ((cpuvm'Flags Sign) not). ].
            
        aVm setRegister &register:12 &value:flags.   
    ]
}
