import system'collections.
import system'routines.
import system'dynamic.

import extensions.
import extensions'scripting.
import extensions'dynamic.

import extensions'dynamic'expressions.

class TapeAssembler
{    
    stack                theBrackets.
    list<TapeExpression> theTape.
    
    constructor new
    [
        theBrackets := Stack new.
        theTape := list<TapeExpression>().
        
        theTape append(TapeExpression Declaring("ptr")).
        theTape append(TapeExpression Assigning("ptr", TapeExpression Constant(0))).
    ]
    
    constructor new : assembly_program
        <= new;
    [
        assembly_program($self).
    ]    
    
    open
    [
        theBrackets push(theTape).
        theTape := list<TapeExpression>().
    ]
    
    close
    [
        var loop := TapeExpression Loop(
                          TapeExpression MessageCall(
                             TapeExpression MessageCall(
                                TapeExpression Variable("tape"),
                                "getAt",
                                TapeExpression Variable("ptr")
                             ),
                             "notequal",
                             TapeExpression Constant($0)),
                          TapeExpression Code(theTape array)).
                          
        theTape := theBrackets pop.
        theTape append(loop).                        
    ]
    
    input
    [
        theTape append(TapeExpression MessageCall(
                        TapeExpression Variable("tape"),
                        "setAt",
                        TapeExpression Variable("ptr"),
                        TapeExpression MessageCall(
                           TapeExpression Constant(console),
                           "readChar"
                        ))).
    ]
    
    output
    [
        theTape append(TapeExpression MessageCall(
                                 TapeExpression Constant(console), 
                                 "write",
                                 TapeExpression MessageCall(
                                   TapeExpression Variable("tape"),
                                   "getAt",
                                   TapeExpression Variable("ptr")
                                 ))).
    ]
    
    next
    [
        theTape append(TapeExpression Assigning(
            "ptr",
            TapeExpression MessageCall(
                TapeExpression Variable("ptr"),
                "add",
                TapeExpression Constant(1)))).
    ]
    
    previous
    [
        theTape append(TapeExpression Assigning(
            "ptr",
            TapeExpression MessageCall(
                TapeExpression Variable("ptr"),
                "subtract",
                TapeExpression Constant(1)))).
    ]
    
    increase
    [
        theTape append(TapeExpression MessageCall(
                                TapeExpression Variable("tape"),
                                "setAt",
                                TapeExpression Variable("ptr"), 
                                TapeExpression MessageCall(
                                    TapeExpression Constant(CharValue),
                                    "new",
                                    TapeExpression MessageCall(
                                        TapeExpression MessageCall(
                                            TapeExpression Constant(convertor),
                                            "toInt",
                                            TapeExpression MessageCall(
                                                TapeExpression Variable("tape"),
                                                "getAt",
                                                TapeExpression Variable("ptr"))
                                        ),
                                        "add",
                                        TapeExpression Constant(1))))).
    ]
    
    decrease
    [
        theTape append(TapeExpression MessageCall(
                                TapeExpression Variable("tape"),
                                "setAt",
                                TapeExpression Variable("ptr"), 
                                TapeExpression MessageCall(
                                    TapeExpression Constant(CharValue),
                                    "new",
                                    TapeExpression MessageCall(
                                        TapeExpression MessageCall(
                                            TapeExpression Constant(convertor),
                                            "toInt",
                                            TapeExpression MessageCall(
                                                TapeExpression Variable("tape"),
                                                "getAt",
                                                TapeExpression Variable("ptr"))
                                        ),
                                        "subtract",
                                        TapeExpression Constant(1))))).

    ]
    
    get
    [
        var program := TapeExpression Singleton(
                TapeExpression Method(
                   "eval",
                   TapeExpression Code(theTape array),
                   TapeExpression Parameter("tape"))).
                   
        var o := (program compiled)().
                   
        ^(:tape) [ o eval(tape) ]
    ]
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

symbol program = program3.

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
    var bfProgram := TapeAssembler new;
        input; open; output; input; close; get.
                    
    var bfTape := Array new:1024; populate(:n)<int>($0).
    
    bfProgram eval:bfTape.
].

symbol program3bis =
[
    var bfAssemblyProgram := scriptEngine 
        load path:"asmrules.es";
        eval (">,[.,]").

    var bfProgram := TapeAssembler new(bfAssemblyProgram); get.

    var bfTape := Array new:1024; populate(:n)<int>($0).

    bfProgram(bfTape).
].

symbol program3 =
[
    if ('program'arguments length == 1)
        [ console write:"Please provide the path to the file to interpret". AbortException new; raise. ].

    var bfAssemblyProgram := scriptEngine 
        load path:"asmrules.es";
        eval path('program'arguments[1]).

    var bfProgram := TapeAssembler new(bfAssemblyProgram); get.

    var bfTape := Array new:1024; populate(:n)<int>($0).

    bfProgram(bfTape).
].

symbol program4 =
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
