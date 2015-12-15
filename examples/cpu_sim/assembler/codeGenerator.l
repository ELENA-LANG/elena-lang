#import system.
#import system'routines.
#import system'collections.
#import extensions.

#import assembler'Assemblers.

#symbol mnemonics
    = (
        AssemblerAdd,AssemblerAddi,AssemblerAddu,AssemblerBl,AssemblerBr,AssemblerCmp,AssemblerCmpi,AssemblerData,AssemblerDiv,AssemblerDivu,
        AssemblerLb,AssemblerLi,AssemblerLw,AssemblerMov,AssemblerMul,AssemblerMultu,AssemblerOr,AssemblerPop,AssemblerPush,
        AssemblerRet,AssemblerSb,AssemblerSli,AssemblerSri,AssemblerSub,AssemblerSubi,AssemblerSubu,AssemblerSw
    ).
    

#class CodeGenerator
{
    #field output.
    #field assemblers.
    
    #constructor new : aOutput
    [
        assemblers := mnemonics.
        output := aOutput.
    ]
    
    #method assemble : instructions
    [
        instructions run &each: instruction
        [
            instruction castType:
                {
                    eval &instruction:i
                    [
//                        #var menmonic = 
                        
//                    #var tmp := $self searchForMnemonics:(ins Name).
//                    ($nil == tmp)
//                        ? [ console writeLine:(ins Name + " not found"). ]
//                        ! [ tmp assemble &codeGen:$self &instruction:ins &byteCodeStream:output. ].
                    ]
                    
                    eval &label : l
                    [
                    ]
                }
            
//            (ins Type) =>
//                (assembler'TypeInstruction Instruction)
//                ? [
//                  ]
//                
//                (assembler'TypeInstruction Label)
//                ? [
//                    #var cpuSymbol := assembler'CpuSymbol new:ins Name.
//                    output emit &cpuSymbol:cpuSymbol.
//                  ].
        ].
    ]
    
//    #method searchForMnemonics : aMn
//    [        
//        #var i := 0.
//        #var j := 0.
//        #loop (i < assemblers length)?
//        [
//            #var asb := assemblers@i.
//            #loop (j < asb Mnemonic length)?
//            [
//                ((asb Mnemonic)@j == aMn)
//                    ? [ ^assemblers@i. ].
//                j := j + 1.
//            ].
//            i := i + 1.
//            j := 0.
//        ].
//        ^$nil.
//    ]
}