#import system.

#class InstructionBr
{
    #constructor new []

	#method interpret &ins:aInstruction &cpuvm:aVm
    [
        #var(type:byte) code.
        #var(type:long) jump.
        
        code := ((aInstruction shift &index:08) and: 00Fh).
        jump := ((aInstruction shift &index:16) and: 0FFFFh).
        
        #var flags := aVm getRegister:12.
        #var shouldJump := true.
        #var ip := aVm getRegister:15.
        ip := ((ip + jump) and: 00FFFFh) int.
        
        (code int) =>
            (cpuvm'ConditionCode Carry)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Carry)) != 0). ]
                
            (cpuvm'ConditionCode Zero)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Zero)) != 0). ]
                
            (cpuvm'ConditionCode Equal) 
                ? [ shouldJump := ((flags and: (cpuvm'Flags Zero)) != 0). ]   
             
            (cpuvm'ConditionCode NotEqual)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Zero)) == 0). ]
                
            (cpuvm'ConditionCode GreaterThan)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Sign)) == 0). ]
                
            (cpuvm'ConditionCode GreaterThanOrEqual)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Sign) == 0) or: (flags and: (cpuvm'Flags Zero) != 0)). ]
                
            (cpuvm'ConditionCode Sign)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Zero)) != 0). ]
            
            (cpuvm'ConditionCode LessThan)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Zero)) != 0). ]
                
            (cpuvm'ConditionCode LessThanOrEqual)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Sign) != 0) or: (flags and: (cpuvm'Flags Zero) == 0)). ]
                
            (cpuvm'ConditionCode Overflow)
                ? [ shouldJump := ((flags and: (cpuvm'Flags Overflow)) != 0). ]
                
            ! [ shouldJump := true. ].
            
        (shouldJump)?
            [ aVm setRegister &register:15 &value:ip. ].
    ]
}