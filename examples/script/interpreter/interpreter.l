#import system'collections.
#import system'routines.
#import system'dynamic.

#import extensions.
#import extensions'scripting.

// --- Tape ---

class BFTape
{
    object theArray.
    object thePointer.
    object theBrackets.

    constructor new:aLength
    [
        theArray := Array new:aLength set &every: (&int:n) [ Integer new:0 ].
        
        thePointer := Integer new:0.
        
        theBrackets := Stack new.
    ]

    append
    [
         (theArray@thePointer) += 1.
    ]
    
    reduce
    [
         (theArray@thePointer) -= 1.
    ]
    
    next
    [
        thePointer += 1.
    ]

    previous
    [
        thePointer -= 1.
    ]
    
    push : bookmark
    [
        theBrackets push:bookmark.
    ]
    
    pop
        => theBrackets.
    
    input
    [
        theArray@thePointer := console readChar toInt.
    ]
    
    output
    [
        console write:((theArray@thePointer) toChar).
    ]

    check = theArray@thePointer != 0.
}

class TapeAssembler
{    
    object theBrackets.
    object theTape.
    
    constructor new
    [
        theBrackets := Stack new.
        theTape := ArrayList new.
        
        theTape append:(Integer new).
    ]
    
    open
    [
        theBrackets push:(theTape length).
    ]
    
    close
    [
        theTape
            append:3
            append:%"tapeOp.var[]"
            append:2
            append:%"tapeOp.var[]"
            append:%"getAt[1]"
            append:0
            append:%"notequal[1]"
            append:(theBrackets pop)
            append:%"tapeOp.jumpif[]".
    ]
    
    input
    [
        theTape
            append:console
            append:%"readChar[0]"
            append:%"convertorOp.toInt[0]"
            append:3
            append:%"tapeOp.var[]"
            append:2
            append:%"tapeOp.var[]"
            append:%"setAt[2]".
    ]
    
    output
    [
        theTape
            append:3
            append:%"tapeOp.var[]"
            append:2
            append:%"tapeOp.var[]"
            append:%"getAt[1]"
            append:%"convertorOp.toChar[0]"
            append:console
            append:%"write[1]".
    ]
    
    next
    [
        theTape
            append:1
            append:3
            append:%"tapeOp.var[]"
            append:%"append[1]".
    ]
    
    previous
    [
        theTape
            append:1
            append:3
            append:%"tapeOp.var[]"
            append:%"reduce[1]".
    ]
    
    increase
    [
        theTape
            append:1
            append:3
            append:%"tapeOp.var[]"
            append:2
            append:%"tapeOp.var[]"
            append:%"getAt[1]"
            append:%"add[1]"
            append:3
            append:%"tapeOp.var[]"
            append:2
            append:%"tapeOp.var[]"
            append:%"setAt[2]".
    ]
    
    decrease
    [
        theTape
            append:1
            append:3
            append:%"tapeOp.var[]"
            append:2
            append:%"tapeOp.var[]"
            append:%"getAt[1]"
            append:%"subtract[1]"
            append:3
            append:%"tapeOp.var[]"
            append:2
            append:%"tapeOp.var[]"
            append:%"setAt[2]".
    ]
    
    get
        = Tape new &array:theTape.
}

class TapeCGenerator
{    
    object theOutput.
    
    constructor new
    [
        theOutput := system'text'TextBuilder new.
    ]
    
    open
    [
        theOutput writeLine:"while (*p) {".
    ]

    close
    [
        theOutput writeLine:"}".
    ]
    
    input
    [
        theOutput writeLine:"*p = getchar();".
    ]
    
    output
    [
        theOutput writeLine:"putchar(*p);".
    ]
    
    next
    [
        theOutput writeLine:"++p;".
    ]
    
    previous
    [
        theOutput writeLine:"--p;".
    ]

    increase
    [
        theOutput writeLine:"++*p;".
    ]
    
    decrease
    [
        theOutput writeLine:"--*p;".
    ]
    
    get
    [
        theOutput insert:"void execute(char* p) {" &at:0.
        theOutput writeLine:"}".
        
        ^ theOutput literal.
    ]
}

symbol program =
[
    // [,.]
    
    var bfProgram := Tape(
        Integer new,
        console, %"readChar[0]", %"convertorOp.toInt[0]", 3, %"tapeOp.var[]", 2, %"tapeOp.var[]", %"setAt[2]",            
        3, %"tapeOp.var[]", 2, %"tapeOp.var[]", %"getAt[1]", %"convertorOp.toChar[0]", console, %"write[1]",        
        3, %"tapeOp.var[]", 2, %"tapeOp.var[]", %"getAt[1]", 0, %"notequal[1]", 1, %"tapeOp.jumpif[]").
        
    var bfTape := Array new:1024 set &every:(&int:n) [ Integer new ].
    
    bfProgram eval:bfTape.
].

symbol program2 =
[
    ('program'arguments length == 1) ?
        [ console write:"Please provide the path to the file to interpret". AbortException new raise. ].

    var bfProgram := scriptEngine 
        load &path:"rules.es"
        eval &path:('program'arguments@1).

    bfProgram eval:(BFTape new:1024).
].

symbol program3 =
[
    var bfProgram := TapeAssembler new
        open input output close get.
                    
    var bfTape := Array new:1024 set &every:(&int:n) [ 0 ].
    
    bfProgram eval:bfTape.
].

symbol program4 =
[
    if ('program'arguments length == 1)
        [ console write:"Please provide the path to the file to interpret". AbortException new raise. ].

    var bfAssemblyProgram := scriptEngine 
        load &path:"asmrules.es"
        eval &path:('program'arguments@1).

    var bfProgram := bfAssemblyProgram eval:(TapeAssembler new).

    var bfTape := Array new:1024 set &every:(&int:n) [ 0 ].

    bfProgram eval:bfTape.
].

symbol program5 =
[
    if ('program'arguments length == 1)
        [ console write:"Please provide the path to the file to interpret". #throw AbortException new. ].

    var bfAssemblyProgram := scriptEngine 
        load &path:"asmrules.es"
        eval &path:('program'arguments@1).

    var bfProgram := bfAssemblyProgram eval:(TapeCGenerator new).

    var len := bfProgram length.

    console writeLine:bfProgram.
].
