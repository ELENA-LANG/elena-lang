import system'collections.
import system'routines.
import system'dynamic.

import extensions.
import extensions'scripting.
import extensions'dynamic.

import extensions'dynamic'expressions.

// --- Tape ---

class BFTape
{
    object theArray.
    object thePointer.
    object theBrackets.

    constructor new:aLength
    [
        theArray := Array new:aLength; populate(:n)<int>(Integer new:0).
        
        thePointer := Integer new:0.
        
        theBrackets := Stack new.
    ]

    append
    [
         theArray[thePointer] append:1.
    ]
    
    reduce
    [
         theArray[thePointer] reduce:1.
    ]
    
    next
    [
        thePointer append:1.
    ]

    previous
    [
        thePointer reduce:1.
    ]
    
    push : bookmark
    [
        theBrackets push:bookmark.
    ]
    
    pop
        => theBrackets.
    
    input
    [
        theArray[thePointer] := console readChar; toInt.
    ]
    
    output
    [
        console write(theArray[thePointer] toChar).
    ]

    check = theArray[thePointer] != 0.
}

class TapeAssembler
{    
    object theBrackets.
    object theTape.
    
    constructor new
    [
        theBrackets := Stack new.
        theTape := ArrayList new.
        
        theTape append(Integer new).
    ]
    
    open
    [
        theBrackets push(theTape length).
    ]
    
    close
    [
        theTape
            append:3;
            append:%"tapeOp.tape_var[]";
            append:2;
            append:%"tapeOp.tape_var[]";
            append:%"getAt[1]";
            append:0;
            append:%"notequal[1]";
            append(theBrackets pop);
            append:%"tapeOp.tape_jumpif[]".
    ]
    
    input
    [
        theTape
            append:console;
            append:%"readChar[0]";
            append:%"convertorOp.toInt[0]";
            append:3;
            append:%"tapeOp.tape_var[]";
            append:2;
            append:%"tapeOp.tape_var[]";
            append:%"setAt[2]".
    ]
    
    output
    [
        theTape
            append:3;
            append:%"tapeOp.tape_var[]";
            append:2;
            append:%"tapeOp.tape_var[]";
            append:%"getAt[1]";
            append:%"convertorOp.toChar[0]";
            append:console;
            append:%"write[1]".
    ]
    
    next
    [
        theTape
            append:1;
            append:3;
            append:%"tapeOp.tape_var[]";
            append:%"append[1]".
    ]
    
    previous
    [
        theTape
            append:1;
            append:3;
            append:%"tapeOp.tape_var[]";
            append:%"reduce[1]".
    ]
    
    increase
    [
        theTape
            append:1;
            append:3;
            append:%"tapeOp.tape_var[]";
            append:2;
            append:%"tapeOp.tape_var[]";
            append:%"getAt[1]";
            append:%"add[1]";
            append:3;
            append:%"tapeOp.tape_var[]";
            append:2;
            append:%"tapeOp.tape_var[]";
            append:%"setAt[2]".
    ]
    
    decrease
    [
        theTape
            append:1;
            append:3;
            append:%"tapeOp.tape_var[]";
            append:2;
            append:%"tapeOp.tape_var[]";
            append:%"getAt[1]";
            append:%"subtract[1]";
            append:3;
            append:%"tapeOp.tape_var[]";
            append:2;
            append:%"tapeOp.tape_var[]";
            append:%"setAt[2]".
    ]
    
    get
        = Tape new array:theTape.
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
        theOutput printLine:"while (*p) {".
    ]

    close
    [
        theOutput printLine:"}".
    ]
    
    input
    [
        theOutput printLine:"*p = getchar();".
    ]
    
    output
    [
        theOutput printLine:"putchar(*p);".
    ]
    
    next
    [
        theOutput printLine:"++p;".
    ]
    
    previous
    [
        theOutput printLine:"--p;".
    ]

    increase
    [
        theOutput printLine:"++*p;".
    ]
    
    decrease
    [
        theOutput printLine:"--*p;".
    ]
    
    get
    [
        theOutput insert:"void execute(char* p) {" at:0.
        theOutput printLine:"}".
        
        ^ theOutput literal.
    ]
}

symbol program = program1.

program0 =
[
    // .[,.]
    
    var a1 := ClosureTape::(
        // declare a singleton
        openSingletonClosure,
        // declare a method
        openMethodClosure,
        newMessageClosure new:"eval",
        newParamTokenClosure new:"tape",
        newParamTokenClosure new:"self",        
        openCodeClosure,                

        // declare a variable - ptr
        openVariableClosure,
         newIdentifierClosure new:"ptr",
        closeClosure,        
        
        // ptr := 0
        openExpressionClosure,
         newIdentifierClosure new:"ptr",
         newAssignClosure,
         newNumericClosure new:"0",
        closeClosure,
        
        // tape[ptr] := console readChar
        openExpressionClosure,
         newIdentifierClosure new:"tape",
         newMessageClosure new:"setAt",
         newIdentifierClosure new:"ptr",
         openExpressionClosure,
          newReferenceClosure new:"system'console",
          newMessageClosure new:"readChar",
         closeClosure,
        closeClosure,

        // while (tape[ptr]!=0) [
        openLoopClosure,
         openExpressionClosure,
          ifClosure,
          openExpressionClosure,
           openExpressionClosure,
            newIdentifierClosure new:"tape",
            newMessageClosure new:"getAt",
            newIdentifierClosure new:"ptr",
            nextClosure,
            newMessageClosure new:"notequal",
            newNumericClosure new:"0",
           closeClosure,  
          closeClosure,
          openExpressionClosure,
           openCodeClosure,
            // console write(tape[ptr])
            openExpressionClosure,
             newReferenceClosure new:"system'console",             
             newMessageClosure new:"write",
             openExpressionClosure,
              newIdentifierClosure new:"tape",
              newMessageClosure new:"getAt",
              newIdentifierClosure new:"ptr",
             closeClosure,
            closeClosure,
            
            // tape[ptr] := console readChar
            openExpressionClosure,
             newIdentifierClosure new:"tape",
             newMessageClosure new:"setAt",
             newIdentifierClosure new:"ptr",
             openExpressionClosure,
              newReferenceClosure new:"system'console",
              newMessageClosure new:"readChar",
             closeClosure,
            closeClosure,
            
           closeClosure,
          closeClosure,
         closeClosure,
        closeClosure,

        closeClosure,        
        closeClosure,
        closeClosure).

    var bf_tape := Array new:1024; populate(:n)($0).
    var p := TapeAssembly new(a1); eval.
    p eval(bf_tape).
].    

program1 =
[
    var c := TapeExpression Singleton(
                TapeExpression Method(
                   "eval",
                   TapeExpression Code(
                      TapeExpression Declaring("ptr"),
                      // ptr := 0
                      TapeExpression Assigning(
                        "ptr",
                        TapeExpression Constant(0)
                      ),
                      
                      // tape[ptr] := console readChar
                      TapeExpression MessageCall(
                        TapeExpression Variable("tape"),
                        "setAt",
                        TapeExpression Variable("ptr"),
                        TapeExpression MessageCall(
                           TapeExpression Constant(console),
                           "readChar"
                        )),
                      
                      // while (tape[ptr]!=0) [
                      TapeExpression Loop(
                          TapeExpression MessageCall(
                             TapeExpression MessageCall(
                                TapeExpression Variable("tape"),
                                "getAt",
                                TapeExpression Variable("ptr")
                             ),
                             "notequal",
                             TapeExpression Constant($0)),
                          TapeExpression Code(
                              // console write(tape[ptr])  
                              TapeExpression MessageCall(
                                 TapeExpression Constant(console), 
                                 "write",
                                 TapeExpression MessageCall(
                                   TapeExpression Variable("tape"),
                                   "getAt",
                                   TapeExpression Variable("ptr")
                                 )),
                              // tape[ptr] := console readChar
                              TapeExpression MessageCall(
                                TapeExpression Variable("tape"),
                                "setAt",
                                TapeExpression Variable("ptr"), 
                                TapeExpression MessageCall(
                                    TapeExpression Constant(console),
                                    "readChar"))))),
                      TapeExpression Parameter("tape"))).

    var bf_tape := Array new:1024; populate(:n)($0).
    var o := (c compiled)().   
    
    o eval(bf_tape).
].

symbol program2 =
[
    if ('program'arguments length == 1)
        [ console write:"Please provide the path to the file to interpret". AbortException new; raise ].

    var bfProgram := scriptEngine 
        load path:"rules.es";
        eval path('program'arguments[1]).

    bfProgram eval(BFTape new:1024).
].

symbol program3 =
[
    var bfProgram := TapeAssembler new;
        open; input; output; close; get.
                    
    var bfTape := Array new:1024; populate(:n)<int> [ ^0 ].
    
    bfProgram eval:bfTape.
].

symbol program4 =
[
    if ('program'arguments length == 1)
        [ console write:"Please provide the path to the file to interpret". AbortException new; raise. ].

    var bfAssemblyProgram := scriptEngine 
        load path:"asmrules.es";
        eval path('program'arguments[1]).

    var bfProgram := bfAssemblyProgram eval(TapeAssembler new).

    var bfTape := Array new:1024; populate(:n)<int>( 0 ).

    bfProgram eval:bfTape.
].

symbol program5 =
[
    if ('program'arguments length == 1)
        [ console write:"Please provide the path to the file to interpret". AbortException new; raise ].

    var bfAssemblyProgram := scriptEngine 
        load path:"asmrules.es";
        eval path:('program'arguments[1]).

    var bfProgram := bfAssemblyProgram eval:(TapeCGenerator new).

    var len := bfProgram length.

    console writeLine:bfProgram.
].
