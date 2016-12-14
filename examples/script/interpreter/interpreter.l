#import system.
#import system'routines.
#import system'dynamic.
#import extensions.

// --- Tape ---

#class BFTape
{
    #field theArray.
    #field thePointer.

    #constructor new:aLength
    [
        theArray := Array new:aLength set &every: (&int:n) [ Integer new:0 ].
        
        thePointer := Integer new:0.
    ]

//    #method bf_tape = $self.
//
//    #method append
//    [
//         (theArray@thePointer) += 1.
//    ]
//    
//    #method reduce
//    [
//         (theArray@thePointer) -= 1.
//    ]
//    
//    #method next
//    [
//        thePointer += 1.
//    ]
//
//    #method previous
//    [
//        thePointer -= 1.
//    ]
    
    #method input
    [
        theArray@thePointer := console readChar toInt.
    ]
    
    #method output
    [
        console write:((theArray@thePointer) toChar).
    ]

    #method check = theArray@thePointer != 0.
}

//// --- LoopInterpreter ---
//
//#class LoopInterpreter
//{
//    #field theLoopBody.
//    #field theTape.
//    
//    #constructor new &tape:aTape
//    [
//        theTape := aTape.
//        theLoopBody := String new.
//    ]
//    
//    #method bf_tape = theTape bf_tape.
//
//    #method append
//    [
//         theLoopBody += "+".
//    ]
//    
//    #method reduce
//    [
//         theLoopBody += "-".
//    ]
//    
//    #method next
//    [
//         theLoopBody += ">".
//    ]
//
//    #method previous
//    [
//         theLoopBody += "<".
//    ]
//    
//    #method input
//    [
//         theLoopBody += ",".
//    ]
//    
//    #method output
//    [
//         theLoopBody += ".".
//    ]
//    
//    #method repeatUntil
//    [
//        theTape run: aTape [ interpreter'Interpreter new:aTape eval:theLoopBody. ].
//            
//        ^ theTape.
//    ]
//}
//
//// --- Interpreter ---
//
//#class Interpreter
//{
//    #field theTape.
//
//    #constructor new : aTape
//    [
//        theTape := aTape.
//    ]
//
//    #method eval : anObject
//    [
//        $self eval &literal::anObject.
//    ]
//        
//    #method eval &literal:aLiteral
//    [
//        control foreach:aLiteral &do: ch [ self eval &char:ch. ].
//    ]
//
//    #method eval &char:aChar
//    [
//        aChar =>
//            ">" ? [ theTape next ]
//            "<" ? [ theTape previous ]
//            "+" ? [ theTape append ]
//            "-" ? [ theTape reduce ]
//            "." ? [ theTape output. ]
//            "," ? [ theTape input. ]
//            "[" ? [ theTape := LoopInterpreter new &tape:theTape. ]
//            "]" ? [ theTape := theTape repeatUntil. ].
//    ]
//}
//
//// --- Program ---
//
//#symbol program = 
//[
//    ('program'arguments length == 1)?
//        [  console write:"Please provide the path to the file to interpret". #throw BreakException new. ].
//    
//    textFileControl forEachLine:('program'arguments@1) &do:(Interpreter new:(BFTape new &length:1024)).
//].
//
//#symbol program2 =
//[
////    console readChar.
//    
//    ('program'arguments length == 1)?
//        [  console write:"Please provide the path to the file to interpret". #throw BreakException new. ].
//
//    scriptEngine load &path:"rules.es"
//        | if &ScriptFailed : ex [ console writeLine:(ex Message). #break nil. ].
//
//    #var aBFTape := BFTape new &length:1024.
//    
//    #var aSourceTape := scriptEngine load &path:('program'arguments@1)
//        | if &ScriptFailed : ex [ console writeLine:(ex Message). #break nil. ].
//    
//    aSourceTape eval:aBFTape.
//].

#symbol program =
[
    //[.,]
    
    #var program := Tape(2, %"tapeOp.var[]", %"input[0]", %"output[0]", 0, %"tapeOp.stack[]", %"check[0]", 2, %"tapeOp.jumpif[1,]").
    
    program eval:(BFTape new:1024).
].