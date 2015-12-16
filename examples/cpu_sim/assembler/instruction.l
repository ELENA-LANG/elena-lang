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
    
    #method Name = theName.
    
    #method Operands = theOperands.
    
//    #method Line = line.
}

#class CpuInstruction
{
    #field theAddress.
    #field theSymbol.
    
    #field theOpcode.
    #field theReg1.
    #field theReg2.
    #field theImmediate.
    #field theImmediateIsRegister.
    #field theOperandIsConditionCode.
    #field theRelative.
    
//    #field type.
    
    #constructor $init
    [
        theImmediate := 0.
        theImmediateIsRegister := false.
        theOperandIsConditionCode := false.
        theRelative := false.
//        type := assembler'TypeInstruction CpuInstruction.
    ]
    
    #constructor new &opcode:aOpcode &cpuRegister:aRegister
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theReg1 := aRegister byte.
    ]
    
    #constructor new &opcode:aOpcode &regList:aRegisterList
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theImmediate := aRegisterList.
    ]    
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &cpuReg2:aReg2
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theReg1 := aReg1 byte.
        theReg2 := aReg2 byte.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &cpuReg2:aReg2 &cpuReg3:aReg3
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theReg1 := aReg1 byte.
        theReg2 := aReg2 byte.
        theImmediate := aReg3 int.
        theImmediateIsRegister := true.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &cpuReg2:aReg2 &imm:aImm
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theReg1 := aReg1 byte.
        theReg2 := aReg2 byte.
        theImmediate := aImm int.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &cpuReg2:aReg2 &strImm:aImm
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theReg1 := aReg1 byte.
        theReg2 := aReg2 byte.
        theSymbol := aImm literal.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &imm:aImm
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theReg1 := aReg1 byte.
        theImmediate := aImm int.
    ]
    
    #constructor new &opcode:aOpcode &cpuReg1:aReg1 &strImm:aImm
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theReg1 := aReg1 byte.
        theSymbol := aImm.
    ]
    
    #constructor new &opcode:aOpcode &conditionCode:code &sym:aSym &relative:aRelative
        <= ($init)
    [
        theOpcode := aOpcode byte.
        theReg1 := code byte.
        theSymbol := aSym.
        theOperandIsConditionCode := true.
        theRelative := aRelative.
    ]
    
//    #method resolveSymbol &sym:aSym &address:aAddr
//    [
//        (symbol == aSym)
//        ? [
//            (relative == true)
//                ? [ immediate := ((aAddr short) - ((address short)+4)). ]
//                ! [ immediate := aAddr. ].
//          ]
//        ! [ ^false. ].
//    ]
//    
//    #method toMachineCode
//    [
//        #var final := opcode int.
//        (immediateIsRegister == true)
//            ? [ final := final or: 080h. ].
//        (operandIsConditionCode == true)
//            ? [ final := final or: 040h. ].
//            
//        final := final or: ((reg1 int) shift &index:(-8)).
//        final := final or: ((reg2 int) shift &index:(-12)).
//        final := final or: (immediate   shift &index:(-16)).
//        ^final.
//    ]
//    
//    #method setPosition : addr
//    [
//        address := addr.
//    ]
//    
//    #method writeData : stream
//    [
//        #var(type:bytearray,size:4) auxWriter.
//        #var(type:int) tmp := $self toMachineCode.
//        auxWriter write &int:0 &int:tmp.
//            
//        #var(type:byte)v1.
//        auxWriter read &int:0 &vbyte:v1.
//        stream write &byte:(v1).
//            
//        auxWriter read &int:1 &vbyte:v1.
//        stream write &byte:(v1).
//            
//        auxWriter read &int:2 &vbyte:v1.
//        stream write &byte:(v1).
//          
//        auxWriter read &int:3 &vbyte:v1.
//        stream write &byte:(v1).
//    ]
    
//    #method GetLength = 4.
    
//    #method Address = address.
//    #method Address : aAddress [ address := aAddress. ]
    
//    #method Symbol = symbol.
    
//    #method Type = type.
}

#class CpuData
{
    #field theData.
//    #field type.
    
    #constructor new &data:data
    [
        #var dataLen := data length.
        theData := Array new &length:(dataLen + dataLen int mod: 4) set &every:(&index:i) [ (i < dataLen) ? [ data@i ] ! [ 0 ] ].
        
        //type := assembler'TypeInstruction CpuData.
    ]
    
//    #method writeData : stream
//    [
//        #var i := 0.
//        #loop (i < data length)?
//        [
//            stream write &byte:((data@i) byte).
//            i := i + 1.
//        ].
//    ]
//    
//    #method setPosition : addr
//    [
//    ]
//    
//    #method GetLength
//    [
//        ^(data length).
//    ]
//    
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
