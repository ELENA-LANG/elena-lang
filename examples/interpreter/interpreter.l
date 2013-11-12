#define system.
#define extensions'text.

// --- Tape ---

#class Tape
{
    #field theArray.
    #field thePointer.

    #constructor new &type'length:aLength
    [
        thePointer := Integer new:0.

        theArray := Array new &type'length:aLength &function: &&:n [ Integer new:0 ].
    ]

    #method type'tape = $self.

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
        (theArray@thePointer) write &type'int:(console readChar).
    ]
    
    #method output
    [
        console write:(CharValue new:(theArray@thePointer)).
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
    
    #method type'tape = theTape type'tape.

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
    
    #method run
    [
        control while:[0 < theTape get ] &do:
            [ interpreter'Interpreter new:(theTape type'tape) eval:theLoopBody. ].
            
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
        $self eval::anObject.
    ]
        
    #method eval &type'widestr:aLiteral
    [
        control foreach:aLiteral &do:$self.
    ]

    #method eval &type'widechar:aChar
    [
        aChar =>
            ">" ? [ theTape next ]
            "<" ? [ theTape previous ]
            "+" ? [ theTape append ]
            "-" ? [ theTape reduce ]
            "." ? [ theTape output. ]
            "," ? [ theTape input. ]
            "[" ? [ theTape := LoopInterpreter new &tape:theTape. ]
            "]" ? [ theTape := theTape run. ].
    ]
}

// --- Program ---

#symbol program = 
[
    ('program'arguments Count == 1)?
        [  console write:"Please provide the path to the file to interpret". #throw BreakException new. ].
    
    textFileControl forEachLine:('program'arguments@1) &do:(Interpreter new:(Tape new &type'length:1024)).
].