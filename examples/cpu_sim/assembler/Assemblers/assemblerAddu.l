#import system.
#import system'collections.

#symbol AssemblerAddu =
{
//    #field mnemonic.
//
//    #constructor new
//    [
//        mnemonic := List new.
//        mnemonic += "addu".
//    ]

    #method assemble &codeGen:aCodeGen &instruction:aIns &byteCodeStream:aStr
    [
//        #var operands := aIns Operands.
//        (operands length != 2)
//            ? [ console writeLine:(aIns Name + " does not take + " + aIns Operands length literal). ^$nil. ].
//        ((((operands@0 tokenType == assembler'TokenType Register) or: 
//           (operands@1 tokenType == assembler'TokenType Register) != true) or: 
//           (operands@2 tokenType == assembler'TokenType Register) != true))
//           ? [ console writeLine:("Unsupported addressing mode for instruction '" + aIns Name + "'"). ^$nil. ].
//           
//        #var reg1 := (operands@0 token) Register.
//        #var reg2 := (operands@1 token) Register.
//        #var reg3 := (operands@2 token) Register.
    ]

//    #method Mnemonic = mnemonic.
}.
