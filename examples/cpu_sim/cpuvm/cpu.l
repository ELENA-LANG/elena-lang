#import system.
#import system'io.
#import system'routines.
#import system'collections.
#import extensions.

#symbol(const, type:int) UserMode           = 1.
#symbol(const, type:int) EnablePaging       = 2.
#symbol(const, type:int) EnableBoundryCheck = 4.

#class Cpu
{
    #field physicalMemory.
    #field registersSupervisor.
    #field registersUser.
    #field halted.
    #field speed.
    #field instructions.
    #field controlRegister.
    #field currentInstruction.
    
    #constructor new
    [
        physicalMemory := cpuvm'MemoryPhysical new.
        instructions := Dictionary new.
        registersUser := IntArray new &int:16.
        registersSupervisor := IntArray new &int:16.
        
        controlRegister := 0.
        halted := false.
        speed := 100000.
        
        $self setOpcodes.      
    ]
    
    #method setOpcodes
    [
        instructions@00h := cpuvm'instructions'InstructionAddi new.
        instructions@80h := cpuvm'instructions'InstructionAdd new.
        instructions@83h := cpuvm'instructions'InstructionSub new.
        instructions@03h := cpuvm'instructions'InstructionSubi new.
        instructions@86h := cpuvm'instructions'InstructionMul new.
        instructions@88h := cpuvm'instructions'InstructionDiv new.
        instructions@59h := cpuvm'instructions'InstructionBr new.
        instructions@5Ah := cpuvm'instructions'InstructionBl new.
        instructions@91h := cpuvm'instructions'InstructionOr new.
        instructions@14h := cpuvm'instructions'InstructionLi new.
        instructions@15h := cpuvm'instructions'InstructionLb new.
        instructions@16h := cpuvm'instructions'InstructionLw new.
        instructions@17h := cpuvm'instructions'InstructionSb new.
        instructions@18h := cpuvm'instructions'InstructionSw new.
        instructions@9Bh := cpuvm'instructions'InstructionCmp new.
        instructions@1Bh := cpuvm'instructions'InstructionCmpi new.
        instructions@21h := cpuvm'instructions'InstructionPush new.
        instructions@22h := cpuvm'instructions'InstructionPop new.
        instructions@0Eh := cpuvm'instructions'InstructionSli new.
        instructions@0Fh := cpuvm'instructions'InstructionSri new.
    ]
    
    #method loadProgram &dataBytes:aProgram
    [
        #var i := 0.
        #loop (i < aProgram length) ?
        [
            physicalMemory writeByte &address:i &byt:(aProgram@i). 
            i := i + 1.
        ].
    ]
    
    #method loadProgram &fileName:aFileName
    [
        #var(type:vbyte) byte := 0.
        #var br := BinaryFileReader new &path:aFileName.   
        #var i := 0.
        #loop (br available) ?
        [
            br read &vbyte:byte.
            physicalMemory writeByte &address:i &byt:(byte). 
            i := i + 1.
        ].
        br close.  
    ]
    
    #method setRegister &register:aRegister &value:aValue
    [
        ((controlRegister and: UserMode) == 0)
            ? [ registersSupervisor@aRegister := aValue. ]
            ! [ registersUser@aRegister := aValue. ].
    ]
    
    #method getRegister : aRegister
    [
        ((controlRegister and: UserMode) == 0)
            ? [ ^registersSupervisor@aRegister. ]
            ! [ ^registersUser@aRegister. ].
    ]
    
    #method doStep
    [
        currentInstruction := $self fetchInstruction.
        halted := $self decodeInstruction:(currentInstruction) not. 
    ]
    
    #method emulate
    [
        #loop (halted != true) ?
        [
            $self cpuTick.
        ].
    ]
    
    #method cpuTick
    [
        #var i := 0.
        //#var instruction := -1.
        #loop ((i < (speed / 100)) and: (halted != true))?
        [
            //instruction := $self fetchInstruction.
            //#var uu := (i literal) padLeft &char:(CharValue new &char:#48) &int:4.
            //console writeLine:(uu + " ---- " + (instruction toLiteral &base:16) + " ---- " + ($self getRegister:15) toLiteral &base:16).
            //halted := $self decodeInstruction:instruction not.
            halted := $self decodeInstruction:($self fetchInstruction) not.
            i := i + 1.
        ].
    ]
    
    #method fetchInstruction
    [
        #var pc := $self getRegister:15.
        $self setRegister &register:15 &value:(pc + 4).
        ^($self getMemory readWord:pc).
        //^(physicalMemory readWord:pc).
    ]
    
    #method decodeInstruction : instruction
    [
        //console writeLine:(instruction toLiteral &base:16).
        
        #var lRet := false.
        #var opcode := instruction and: 0FFh.
        #var inst := instructions@opcode.
        ($nil != inst)?
            [ 
                inst interpret &ins:instruction &cpuvm:($self).
                lRet := true.
            ].
        ^lRet.
    ]
    
    #method reset
    [
        #var i := 0.
        #loop (i < 16)?
        [
            $self setRegister &register:i &value:0.
            i := i + 1.
        ].
        halted := false.
    ]
    
    #method getMemory = physicalMemory.
    #method controlRegister = controlRegister.
    
    #method Halted = halted.
    #method Halted:aValue [ halted := aValue. ]
    
    #method Speed = speed.
    
    #method CurrentInstruction = currentInstruction.
}