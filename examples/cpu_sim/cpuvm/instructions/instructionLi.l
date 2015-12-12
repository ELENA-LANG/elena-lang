#import system.
#import extensions.

#class InstructionLi
{
	#constructor new []

	#method interpret &ins:aInstruction &cpuvm:aVm
    [
        #var(type:byte) destRegister.
        #var(type:short) data.
        
        destRegister := ((aInstruction shift &index:08) and: 00Fh).
        data         := ((aInstruction shift &index:16) and: 0FFFFh).
        
        /*#var(type:bytearray,size:8)dump.
        #var(type:byte)destRegister.
        #var(type:short)data.
        
        dump write &int:0 &long:(aInstruction long).        
        
        dump read &int:1 &vbyte:destRegister.
        dump read &int:2 &vshort:data.*/
        
        aVm setRegister &register:destRegister &value:data.
    ]
}