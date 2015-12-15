#import system.
#import system'io.
#import system'math.
#import system'collections.

#class Instruction
{
    #field theName.
    #field theOperands.
    #field theLineNumber.
    
    #constructor new &name:name &operands:operands &line_number:lineNumber
    [
        theLineNumber := lineNumber.
        theName := name.
        theOperands := operands.
    ]
    
    #method castType : target
        = target eval &instruction:$self.
    
//    #method Name = name.
//    #method Line = line.
//    #method Operands = operands.
//    #method Type = type.
}










#symbol(const,type:int)TY_Label       = 0.
#symbol(const,type:int)TY_Instruction = 1.

// Used to separete data from instruction..
#symbol(const,type:int)T_Data = 0.
#symbol(const,type:int)T_Inst = 1.

#symbol TypeInstruction =
{
    #method Label = TY_Label.
    #method Instruction = TY_Instruction.
    
    #method CpuData = T_Data.
    #method CpuInstruction = T_Inst.
}.

#class CpuData
{
    #field data.
    #field type.
    
    #constructor new &data:aData
    [
        #var size := (aData length + aData length mod: 4) int.
        data := Array new &length:size.
        
        #var i := 0.
        #loop (i < size) ?
        [
            (i < aData length)
                ? [ data@i := aData@i. ]
                ! [ data@i := ByteNumber new. ]. 
            i := i + 1.
        ].
        
        type := assembler'TypeInstruction CpuData.
    ]
    
    #method writeData : stream
    [
        #var i := 0.
        #loop (i < data length)?
        [
            stream write &byte:((data@i) byte).
            i := i + 1.
        ].
    ]
    
    #method setPosition : addr
    [
    ]
    
    #method GetLength
    [
        ^(data length).
    ]
    
    #method Type = type.
}

#class CpuInstruction
{
    #field address.
    #field symbol.
    
    #field opcode.
    #field reg1.
    #field reg2.
    #field immediate.
    #field immediateIsRegister.
    #field operandIsConditionCode.
    #field relative.
    
    #field type.
    
    #method init
    [
        opcode := ByteNumber new.
        reg1 := ByteNumber new.
        reg2 := ByteNumber new.
        immediate := IntNumber new. //ByteArray new &length:4.       
        immediateIsRegister := false.
        operandIsConditionCode := false.
        relative := false.
        type := assembler'TypeInstruction CpuInstruction.
    ]
    
    #constructor new &opcode:aOpcode &cpuRegister:aRegister
    [
        $self init.
        opcode := aOpcode byte.
        reg1 := aRegister byte.
    ]
    
    #constructor new &opcode:aOpcode &regList:aRegisterList
    [
        $self init.
        opcode := aOpcode byte.
        immediate := aRegisterList.
    ]    
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &cpuReg2:aReg2
    [
        $self init.
        opcode := aOpcode byte.
        reg1 := aReg1 byte.
        reg2 := aReg2 byte.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &cpuReg2:aReg2 &cpuReg3:aReg3
    [
        $self init.
        opcode := aOpcode byte.
        reg1 := aReg1 byte.
        reg2 := aReg2 byte.
        immediate := aReg3 int.
        immediateIsRegister := true.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &cpuReg2:aReg2 &imm:aImm
    [
        $self init.
        opcode := aOpcode byte.
        reg1 := aReg1 byte.
        reg2 := aReg2 byte.
        immediate := aImm int.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &cpuReg2:aReg2 &strImm:aImm
    [
        $self init.
        opcode := aOpcode byte.
        reg1 := aReg1 byte.
        reg2 := aReg2 byte.
        symbol := aImm literal.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &imm:aImm
    [
        $self init.
        opcode := aOpcode byte.
        reg1 := aReg1 byte.
        immediate := aImm int.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &strImm:aImm
    [
        $self init.
        opcode := aOpcode byte.
        reg1 := aReg1 byte.
        symbol := aImm.
    ]
    
    #constructor new &opcode:aOpcode &conditionCode:code &sym:aSym &relative:aRelative
    [
        $self init.
        opcode := aOpcode byte.
        reg1 := code byte.
        symbol := aSym.
        operandIsConditionCode := true.
        relative := aRelative.
    ]
    
    #method resolveSymbol &sym:aSym &address:aAddr
    [
        (symbol == aSym)
        ? [
            (relative == true)
                ? [ immediate := ((aAddr short) - ((address short)+4)). ]
                ! [ immediate := aAddr. ].
          ]
        ! [ ^false. ].
    ]
    
    #method toMachineCode
    [
        #var final := opcode int.
        (immediateIsRegister == true)
            ? [ final := final or: 080h. ].
        (operandIsConditionCode == true)
            ? [ final := final or: 040h. ].
            
        final := final or: ((reg1 int) shift &index:(-8)).
        final := final or: ((reg2 int) shift &index:(-12)).
        final := final or: (immediate   shift &index:(-16)).
        ^final.
    ]
    
    #method setPosition : addr
    [
        address := addr.
    ]
    
    #method writeData : stream
    [
        #var(type:bytearray,size:4) auxWriter.
        #var(type:int) tmp := $self toMachineCode.
        auxWriter write &int:0 &int:tmp.
            
        #var(type:byte)v1.
        auxWriter read &int:0 &vbyte:v1.
        stream write &byte:(v1).
            
        auxWriter read &int:1 &vbyte:v1.
        stream write &byte:(v1).
            
        auxWriter read &int:2 &vbyte:v1.
        stream write &byte:(v1).
          
        auxWriter read &int:3 &vbyte:v1.
        stream write &byte:(v1).
    ]
    
    #method GetLength = 4.
    
    #method Address = address.
    #method Address : aAddress [ address := aAddress. ]
    
    #method Symbol = symbol.
    
    #method Type = type.
}