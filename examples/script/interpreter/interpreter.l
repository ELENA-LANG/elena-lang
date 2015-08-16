#define system.
#define system'routines.
#define extensions'text.
#define extensions'dynamic.

// --- Tape ---

#class BFTape
{
    #field theArray.
    #field thePointer.

    #constructor new &length:aLength
    [
        thePointer := Integer new:0.

        theArray := arrayControl new &length:aLength &each: n [ Integer new:0 ].
    ]

    #method bf_tape = $self.

    #method append
    [
         (theArray@thePointer) += 1.
    ]
    
    #method reduce
    [
         (theArray@thePointer) -= 1.
    ]
    
    #method next
    [
        thePointer += 1.
    ]

    #method previous
    [
        thePointer -= 1.
    ]
    
    #method input
    [
        (theArray@thePointer) write &int::(console readChar).
    ]
    
    #method output
    [
        console write:(CharValue new &int::(theArray@thePointer)).
    ]

    #method run : aLoop
    [
        control while:[ 0 < (theArray@thePointer) ] &do: [ aLoop eval:self. ].
    ]
            
    #method get = theArray@thePointer.
}

// --- LoopInterpreter ---

#class LoopInterpreter
{
    #field theLoopBody.
    #field theTape.
    
    #constructor new &tape:aTape
    [
        theTape := aTape.
        theLoopBody := String new.
    ]
    
    #method bf_tape = theTape bf_tape.

    #method append
    [
         theLoopBody += "+".
    ]
    
    #method reduce
    [
         theLoopBody += "-".
    ]
    
    #method next
    [
         theLoopBody += ">".
    ]

    #method previous
    [
         theLoopBody += "<".
    ]
    
    #method input
    [
         theLoopBody += ",".
    ]
    
    #method output
    [
         theLoopBody += ".".
    ]
    
    #method repeatUntil
    [
        theTape run: aTape [ interpreter'Interpreter new:aTape eval:theLoopBody. ].
            
        ^ theTape.
    ]
}

// --- Interpreter ---

#class Interpreter
{
    #field theTape.

    #constructor new : aTape
    [
        theTape := aTape.
    ]

    #method eval : anObject
    [
        $self eval &literal::anObject.
    ]
        
    #method eval &literal:aLiteral
    [
        control foreach:aLiteral &do: ch [ self eval &char:ch. ].
    ]

    #method eval &char:aChar
    [
        aChar =>
            ">" ? [ theTape next ]
            "<" ? [ theTape previous ]
            "+" ? [ theTape append ]
            "-" ? [ theTape reduce ]
            "." ? [ theTape output. ]
            "," ? [ theTape input. ]
            "[" ? [ theTape := LoopInterpreter new &tape:theTape. ]
            "]" ? [ theTape := theTape repeatUntil. ].
    ]
}

// --- Program ---

#symbol program = 
[
    ('program'arguments length == 1)?
        [  console write:"Please provide the path to the file to interpret". #throw BreakException new. ].
    
    textFileControl forEachLine:('program'arguments@1) &do:(Interpreter new:(BFTape new &length:1024)).
].

#symbol program2 =
[
//    console readChar.
    
    ('program'arguments length == 1)?
        [  console write:"Please provide the path to the file to interpret". #throw BreakException new. ].

    scriptEngine load &path:"rules.es"
        | if &ScriptFailed : ex [ console writeLine:(ex Message). #break nil. ].

    #var aBFTape := BFTape new &length:1024.
    
    #var aSourceTape := scriptEngine load &path:('program'arguments@1)
        | if &ScriptFailed : ex [ console writeLine:(ex Message). #break nil. ].
    
    aSourceTape eval:aBFTape.
].