#import forms.
#import system.
#import system'io.
#import system'routines.
#import system'collections.
#import extensions.

#symbol $loadBytes = aProgram 
[
    #var(type:vbyte) byte := 0.
    #var br := BinaryFileReader new &path:aProgram.   
    #var bta := List new.
    #loop (br available) ?
    [
        br read &vbyte:byte.
        bta += byte.
    ].
    br close.  
    ^bta. 
].

#symbol $padZeros = aValue
[
    ^((aValue literal) padLeft &char:(CharValue new &char:#48) &int:8).
].

#symbol $getRegisterName = registerCode
[
    (registerCode int) =>
        11 ? [ ^"LR". ]
        12 ? [ ^"FLAGS". ]
        13 ? [ ^"BP". ]
        14 ? [ ^"SP". ]
        15 ? [ ^"IP". ]
        16 ? [ ^"CTL". ]
        17 ? [ ^"IVT". ]
        ! [ ^(registerCode literal). ].
].

#class Window
{
    #field form.
    // cpu vm instance
    #field cpu.
    #field instructionDisassembler.
    #field theTextField.
        
    // Labels of registers!
    #field listLabelsRegisters.   
    
    #constructor new
    [        
        cpu := cpuvm'Cpu new.
        instructionDisassembler := cpuvm'Disassembler new.
        
        form := SDIDialog new.
        
        #var stepButton := Button new.
        #var loadButton := Button new.
        #var resetButton := Button new.
        theTextField := Memo new.
        
        form controls 
            += stepButton
            += loadButton
            += resetButton
            += theTextField.
        
        form set &caption:"CPU-Simulator -- Debugger".
        form set &x:100 &y:100.
        form set &width:800 &height:600.    
        
        theTextField set &x:180 &y:10.
        theTextField set &width:600 &height:400.
        theTextField set &caption:(""). 
        
        stepButton set &x:10 &y:380.
        stepButton set &width:75 &height:25.
        stepButton set &caption:"Step".
        stepButton set &onClick:args
            [ $self $makeStep. ].
            
        resetButton set &x:10 &y:410.
        resetButton set &width:75 &height:25.
        resetButton set &caption:"Halt".
        resetButton set &onClick:args
            [ $self $resetCpu. ].            

            
        loadButton set &x:10 &y:440.
        loadButton set &width:75 &height:25.
        loadButton set &caption:"Load".
        loadButton set &onClick:args
            [ $self $loadProgram. ].
          
        
        // Generate the labels
        #var zero := $padZeros:"0".
        #var i := 0.
        #var(type:int) y := 10.
        listLabelsRegisters := List new.
        #loop (i <= 17) ?
        [
            #var tmp := Label new.
            listLabelsRegisters += tmp.
            form controls += tmp.
            
            #var regName := "".
            (i <= 10)
            ? [
                regName := "R" + (i literal) + ": 0x" + zero
              ]
            ! [
                regName := ($getRegisterName:i) + ": 0x" + zero.
              ].
              
            tmp set &x:10 &y:(y int).
            tmp set &width:170 &height:30.
            tmp set &caption:(regName).
            y := y + 20.
            i := i + 1.
        ].
    ]
    
    #method $makeStep
    [
        cpu doStep.
        $self updateRegisterLabels.
    ]
    
    #method $loadProgram
    [
        #var openFile := SaveFileDialog new:$self.
        openFile addFilter &caption:"All files" &extension:"*.*".
        openFile set &caption:"Select a file".
        (openFile run) ?
        [   
            $self $resetCpu.
            cpu loadProgram &fileName:(openFile selected).
            $self $disassembleProgram.
        ].
    ]
    
    #method $haltCpu
    [
        //cpu Halted:true.
    ]
    
    #method $startCpu
    [
        /*cpu Halted:false.
        #loop (cpu Halted != true) ?
        [
            #var i := 0. -1.
            #loop ((i < (cpu Speed / 100)) and: (cpu Halted != true))?
            [
                cpu Halted:(cpu decodeInstruction:(cpu fetchInstruction) not).
                $self updateRegisterLabels.
                i := i + 1.
            ].
        ].*/
    ]
    
    #method $resetCpu
    [
        cpu reset.
        $self updateRegisterLabels.
    ]
    
    #method $disassembleProgram
    [
        #var instruction := 0.
        #var continue := true.
        #var disassemblyText := "".
        #var i := 1.
        #loop (continue)?
        [
            instruction := cpu fetchInstruction.
            
            #var tmp := instructionDisassembler disassembleInstruction:(instruction).
            ($nil == tmp) 
                ? [ continue := false. ]
                ! [
                    #var nl := ((i literal) padLeft &char:(CharValue new &char:#48) &int:4). 
                    #var ins := instruction toLiteral &base:16.
                    #var lin := (nl + " | " + ins + " | >>>>> " + tmp + #0Dh + #0Ah).
                    disassemblyText := (disassemblyText + lin). 
                  ].
            i := i + 1.
        ].
        theTextField set &caption:(disassemblyText).
        disassemblyText := "".
        cpu reset.
    ]
    
    #method updateRegisterLabels
    [
        #var i := 0.      
        #loop (i <= 17)?
        [
            #var regValue := "".
            ( i <= 14) 
                ? [ regValue := $padZeros:(((cpu getRegister:i) toLiteral &base:16) literal). ].
            
            #var regName := "".
            (i <= 10)
            ? [
                regName := "R" + (i literal) + ": 0x" + regValue.
              ]
            ! [
                (i >= 15)
                ? [ 
                    regValue := $padZeros:(((cpu getRegister:15) toLiteral &base:16) literal).
                    regName := ($getRegisterName:i) + ": 0x" + regValue.
                  ]
                ! [ regName := ($getRegisterName:i) + ": 0x" + regValue. ].
                regName := ($getRegisterName:i) + ": 0x" + regValue.
              ].
            
            (listLabelsRegisters@i) set &caption:(regName).
            i := i + 1.
        ].
    ]
    
    #method => form.
}