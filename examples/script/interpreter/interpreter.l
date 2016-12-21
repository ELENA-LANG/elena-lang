#import system.
#import system'collections.
#import system'routines.
#import system'dynamic.

#import extensions.
#import extensions'scripting.

// --- Tape ---

#class BFTape
{
    #field theArray.
    #field thePointer.
    #field theBrackets.

    #constructor new:aLength
    [
        theArray := Array new:aLength set &every: (&int:n) [ Integer new:0 ].
        
        thePointer := Integer new:0.
        
        theBrackets := Stack new.
    ]

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
    
    #method push : bookmark
    [
        theBrackets push:bookmark.
    ]
    
    #method pop
        => theBrackets.
    
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

#class TapeAssembler
{    
    #field theBrackets.
    #field theTape.
    
    #constructor new
    [
        theBrackets := Stack new.
        theTape := ArrayList new.
        
        theTape append:(Integer new).
    ]
    
    #method open
    [
        theBrackets push:(theTape length).
    ]
    
    #method close
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
            append:%"tapeOp.jumpif[1,]".
    ]
    
    #method input
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
    
    #method output
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
    
    #method next
    [
        theTape
            append:1
            append:3
            append:%"tapeOp.var[]"
            append:%"append[1]".
    ]
    
    #method previous
    [
        theTape
            append:1
            append:3
            append:%"tapeOp.var[]"
            append:%"reduce[1]".
    ]
    
    #method increase
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
    
    #method decrease
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
    
    #method get
        = Tape new &array:theTape.
}

#symbol program =
[
    // [,.]
    
    #var bfProgram := Tape(
        Integer new,
        console, %"readChar[0]", %"convertorOp.toInt[0]", 3, %"tapeOp.var[]", 2, %"tapeOp.var[]", %"setAt[2]",            
        3, %"tapeOp.var[]", 2, %"tapeOp.var[]", %"getAt[1]", %"convertorOp.toChar[0]", console, %"write[1]",        
        3, %"tapeOp.var[]", 2, %"tapeOp.var[]", %"getAt[1]", 0, %"notequal[1]", 1, %"tapeOp.jumpif[1,]").
        
    #var bfTape := Array new:1024 set &every:(&int:n) [ Integer new ].
    
    bfProgram eval:bfTape.
].

#symbol program2 =
[
    ('program'arguments length == 1) ?
        [ console write:"Please provide the path to the file to interpret". #throw AbortException new. ].

    #var bfProgram := scriptEngine 
        load &path:"rules.es"
        eval &path:('program'arguments@1).

    bfProgram eval:(BFTape new:1024).
].

#symbol program3 =
[
    #var bfProgram := TapeAssembler new
        open input output close get.
                    
    #var bfTape := Array new:1024 set &every:(&int:n) [ 0 ].
    
    bfProgram eval:bfTape.
].