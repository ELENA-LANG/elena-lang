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
    
#class(extension) mnemonicOp
{
    #method checkMnemonic : aName
        = self Mnemonic cast:%eval &to:
            {
                literal : l
                    = aName == l.
                    
                array : a
                    = a ifExists:aName.
            }.
}

#class CodeGenerator
{
    #field theOutput.
    #field theAssemblers.
    
    #constructor new : aOutput
    [
        theAssemblers := mnemonics.
        theOutput := aOutput.
    ]
    
    #method assemble : instructions
    [
        instructions run &each: instruction
        [
            instruction castType:
                {
                    eval &instruction:i
                    [
                        #var mnemonic := theAssemblers seek &each:m [ m checkMnemonic:(i Name) ].
                        
                        ($nil == mnemonic)
                            ? [ feedback raiseError:(i Name):" not found" ]
                            ! [ mnemonic assemble &codeGen:$self &instruction:i &byteCodeStream:theOutput. ].
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