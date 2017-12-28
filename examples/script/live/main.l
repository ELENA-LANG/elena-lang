//import extensions.
//import system'dynamic.
//
//program_static =
//[
//    var n := var<IntNumber>(0).
//    while (n < 10)
//    [
//        console writeLine(n).
//        
//        n append:1.
//    ]
//].
//
//program_dynamic =
//[ 
//    var n := var<IntNumber>(0).
//
//    var p := Tape::(1000, n, %"less[1]", 13, %"tapeOp.tape_jumpifnot[]", n, console, %"writeLine[1]", 1, n, %"append[1]", 0, %"tapeOp.tape_jump[]").
//    
//    p eval.
//].
//
//program =
//[
//    var start_1 := system'calendar'Date now.
//    program_static eval.
//    var end_1 := system'calendar'Date now.
//
//    var start_2 := system'calendar'Date now.
//    program_dynamic eval.
//    var end_2 := system'calendar'Date now.
//        
//    console printLineFormatted("static program takes {0} msec", (end_1 - start_1) milliseconds).
//    console printLineFormatted("dynymic program takes {0} msec", (end_2 - start_2) milliseconds).
//].

//import system'dynamic'expressions.
//
//program =
//[
//    var m := MessageClosure new(%"writeLine[1]").
//    var c := ConstantClosure new("Hello").
//    
//    var o := c(1,2,3).
//    
//    m(console,"Hallo").
//].

import system'dynamic.
import extensions'dynamic.
 
program =
[
    var a1 := ClosureTape::(
        openSingletonClosure,
        openMethodClosure,
        newMessageClosure new:"eval",
        newParamTokenClosure new:"n",
        newParamTokenClosure new:"self",
        openCodeClosure,        
        openExpressionClosure,
        newReferenceClosure new:"system'console",
        newMessageClosure new:"writeLine",
        newIdentifierClosure new:"n",
        closeClosure,        
        closeClosure,
        closeClosure,
        closeClosure).
  
    var o := TapeAssembly new(a1); eval.
    o eval("Hello again").
].
