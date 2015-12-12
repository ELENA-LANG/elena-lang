#import system.

#class InstructionPop
{
	#constructor new []

	#method interpret &ins:aInstruction &cpuvm:aVm
    [
        #var(type:short) registersList.
        registersList := ((aInstruction shift &index:16) and: 0FFFFh).
        
        #var i := 15.
        #loop (i > -1)?
        [
            (((2 shift &index: (i * -1)) and: registersList) != 0)?
                [
                    #var popedValue := $self pop &cpuvm:aVm.
                    aVm setRegister &register:i &value:popedValue.
                ]. 
            i := i - 1.
        ].
    ]
    
    #method pop &cpuvm:aVm
    [
        #var sp := aVm getRegister:14.
        aVm setRegister &register:14 &value:(sp + 4).        
        ^aVm getMemory readWord:sp.    
    ]
}