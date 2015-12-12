#import system.
#import system'routines.
#import system'collections.
#import extensions.

#class CodeGenerator
{
    #field output.
    #field assemblers.
    
    #constructor new : aOutput
    [
        assemblers := List new.
        output := aOutput.
        
        $self registerAssembler.
    ]
    
    #method assemble : instructions
    [
        instructions run &each: ins
        [
            (ins Type) =>
                (assembler'TypeInstruction Instruction)
                ? [
                    #var tmp := $self searchForMnemonics:(ins Name).
                    ($nil == tmp)
                        ? [ console writeLine:(ins Name + " not found"). ]
                        ! [ tmp assemble &codeGen:($self) &instruction:ins &byteCodeStream:(output). ].
                  ]
                
                (assembler'TypeInstruction Label)
                ? [
                    #var cpuSymbol := assembler'CpuSymbol new:ins Name.
                    output emit &cpuSymbol:cpuSymbol.
                  ].
        ].
    ]
    
    #method registerAssembler
    [
        assemblers += (assembler'Assemblers'AssemblerAdd new).
        assemblers += (assembler'Assemblers'AssemblerAddi new).
        assemblers += (assembler'Assemblers'AssemblerAddu new).
        assemblers += (assembler'Assemblers'AssemblerBl new).
        assemblers += (assembler'Assemblers'AssemblerBr new).
        assemblers += (assembler'Assemblers'AssemblerCmp new).
        assemblers += (assembler'Assemblers'AssemblerCmpi new).
        assemblers += (assembler'Assemblers'AssemblerData new).
        assemblers += (assembler'Assemblers'AssemblerDiv new).
        assemblers += (assembler'Assemblers'AssemblerDivu new).
        assemblers += (assembler'Assemblers'AssemblerLb new).
        assemblers += (assembler'Assemblers'AssemblerLi new).
        assemblers += (assembler'Assemblers'AssemblerLw new).
        assemblers += (assembler'Assemblers'AssemblerMov new).
        assemblers += (assembler'Assemblers'AssemblerMul new).
        assemblers += (assembler'Assemblers'AssemblerMultu new).
        assemblers += (assembler'Assemblers'AssemblerOr new).
        assemblers += (assembler'Assemblers'AssemblerPop new).
        assemblers += (assembler'Assemblers'AssemblerPush new).
        assemblers += (assembler'Assemblers'AssemblerRet new).
        assemblers += (assembler'Assemblers'AssemblerSb new).
        assemblers += (assembler'Assemblers'AssemblerSli new).
        assemblers += (assembler'Assemblers'AssemblerSri new).
        assemblers += (assembler'Assemblers'AssemblerSub new).
        assemblers += (assembler'Assemblers'AssemblerSubi new).
        assemblers += (assembler'Assemblers'AssemblerSubu new).
        assemblers += (assembler'Assemblers'AssemblerSw new).
    ]
    
    #method searchForMnemonics : aMn
    [        
        #var i := 0.
        #var j := 0.
        #loop (i < assemblers length)?
        [
            #var asb := assemblers@i.
            #loop (j < asb Mnemonic length)?
            [
                ((asb Mnemonic)@j == aMn)
                    ? [ ^assemblers@i. ].
                j := j + 1.
            ].
            i := i + 1.
            j := 0.
        ].
        ^$nil.
    ]
}