#import system.
#import extensions.

#class InstructionPush
{
	#constructor new []

	#method interpret &ins:aInstruction &cpuvm:aVm
    [
        #var(type:short) registersList.
        registersList := ((aInstruction shift &index:16) and: 0FFFFh).
        
        #var i := 0.
        #loop (i<16)?
        [
            (((2 shift &index: (i * -1)) and: registersList) != 0)?
                [
                    $self push &value:(aVm getRegister:i) &cpuvm:aVm.
                ].
            i := i + 1.
        ].
    ]
    
    #method push &value:aValue &cpuvm:aVm
    [
        #var sp := aVm getRegister:14.
        aVm getMemory writeWord &address:(sp - 4) &word:aValue.
        aVm setRegister &register:14 &value:(sp - 4).
    ]
}