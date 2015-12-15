#import system.
#import system'collections.

#symbol AssemblerPush =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "push".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var operands := aIns Operands.
//        (operands length == 0)
//            ? [ console writeLine:(aIns Name + " does not take " + aIns Operands length literal). ^$nil. ].
//            
//        (operands@0 tokenType != assembler'TokenType Register)
//           ? [ console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'"). ^$nil. ].
//           
//        #var i := 0.
//        #var registerList := 0.
//        #var skip := Boolean new.
//        #loop (i < operands length) ?
//        [
//            skip := false.
//            ((operands@i) tokenType != assembler'TokenType Register)?
//            [
//                console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'").
//                skip := true.
//            ].
//            
//            (skip == false)?
//            [
//                #var reg := (operands@i token) Register.
//                registerList := registerList or: (2 shift &index:(reg * -1)).
//            ].
//            i := i + 1.
//        ].
//        
//        #var opCode := assembler'Opcode Push.
//        #var tmp := assembler'CpuInstruction new &opcode:opCode &regList:registerList.
//        aStr emit &data:tmp.
    ]

//    #method Mnemonic = mnemonic.
}.
