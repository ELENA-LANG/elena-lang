////import extensions.
////import system'dynamic.
////
////program_static =
////[
////    var n := var<IntNumber>(0).
////    while (n < 10)
////    [
////        console writeLine(n).
////        
////        n append:1.
////    ]
////].
////
////program_dynamic =
////[ 
////    var n := var<IntNumber>(0).
////
////    var p := Tape::(1000, n, %"less[1]", 13, %"tapeOp.tape_jumpifnot[]", n, console, %"writeLine[1]", 1, n, %"append[1]", 0, %"tapeOp.tape_jump[]").
////    
////    p eval.
////].
////
////program =
////[
////    var start_1 := system'calendar'Date now.
////    program_static eval.
////    var end_1 := system'calendar'Date now.
////
////    var start_2 := system'calendar'Date now.
////    program_dynamic eval.
////    var end_2 := system'calendar'Date now.
////        
////    console printLineFormatted("static program takes {0} msec", (end_1 - start_1) milliseconds).
////    console printLineFormatted("dynymic program takes {0} msec", (end_2 - start_2) milliseconds).
////].
//
////import system'dynamic'expressions.
////
////program =
////[
////    var m := MessageClosure new(%"writeLine[1]").
////    var c := ConstantClosure new("Hello").
////    
////    var o := c(1,2,3).
////    
////    m(console,"Hallo").
////].
//
//import system'dynamic.
//import extensions.
//import extensions'dynamic.
// 
//public program =
//[
////    var a1 := ClosureTape::(
////        openSingletonClosure,
////        openMethodClosure,
////        newMessageClosure new:"eval",
////        newParamTokenClosure new:"n",
////        newParamTokenClosure new:"self",        
////        openCodeClosure,                
////        
////        openVariableClosure,
////        newIdentifierClosure new:"m",
////        closeClosure,        
////        
////        openExpressionClosure,
////        newIdentifierClosure new:"m",
////        newAssignClosure,
////        newIdentifierClosure new:"n",
////        closeClosure,
////        
////        newLoopClosure,        
////         openExpressionClosure,
////          ifClosure,
////        
////          openExpressionClosure,
////          newReferenceClosure new:"system'true",
////          closeClosure,
////         
////          openExpressionClosure,        
////           openCodeClosure,
////            openExpressionClosure,
////             newIdentifierClosure new:"m",
////             newAssignClosure,        
////             newNumericClosure new:"0",
////            closeClosure,
////           closeClosure,
////          closeClosure,
////                
////         closeClosure,
////        closeClosure,
////        
////        openReturningClosure,        
////        openExpressionClosure,
////        newIdentifierClosure new:"n",
////        newMessageClosure new:"add",
////        newIdentifierClosure new:"m",
////        closeClosure,                
////        closeClosure,                
////        closeClosure,        
////        closeClosure,
////        closeClosure).
//  
//    var a1 := ClosureTape::(
//        openSingletonClosure,
//        openMethodClosure,
//        newMessageClosure new:"eval",
//        newParamTokenClosure new:"n",
//        newParamTokenClosure new:"self",        
//        openCodeClosure,                
//        
//        openVariableClosure,
//        newIdentifierClosure new:"m",
//        closeClosure,        
//
//        openExpressionClosure,
//        newIdentifierClosure new:"m",
//        newAssignClosure,
//        newIdentifierClosure new:"n",
//        closeClosure,
//                
//        openLoopClosure,        
//         openExpressionClosure,
//          ifClosure,
//        
//          openExpressionClosure,
//          newIdentifierClosure new:"m",
//          newMessageClosure new:"greater",
//          newNumericClosure new:"0",
//          closeClosure,
//         
//          openExpressionClosure,        
//           openCodeClosure,
//            openExpressionClosure,
//             newReferenceClosure new:"system'console",
//             newMessageClosure new:"writeLine",
//             newIdentifierClosure new:"m",
//            closeClosure,                        
//            openExpressionClosure,
//             newIdentifierClosure new:"m",
//             newAssignClosure,
//             openExpressionClosure,
//              newIdentifierClosure new:"m",
//              newMessageClosure new:"subtract",
//              newNumericClosure new:"1",
//             closeClosure,
//            closeClosure,
//           closeClosure,
//          closeClosure,
//                
//         closeClosure,
//        closeClosure,
//        
//        closeClosure,        
//        closeClosure,
//        closeClosure).
//  
//    var o := TapeAssembly new(a1); eval.
//    o eval(5).
//].

import system'dynamic'expressions.
import extensions.

singleton Algorithm
{
    execute(n)
    [
        var previous := Expression Constant(n).
        
        var printing := Expression MessageCall(
                                %"writeLine[1]",
                                Expression Constant(console), 
                                previous).
    
        var t1 := printing compiled.
    
        t1().           
    ]
    
    generic execute(V<object> args)
    [
        var previous := Expression Constant(args[0]).
        
        1 till(args length) do(:i)<int>
        [
            previous := Expression MessageCall(
                                        %"add[1]",
                                        previous,
                                        Expression Constant(args[i])).
                        
        ].
        
        var printing := Expression MessageCall(
                                %"writeLine[1]",
                                Expression Constant(console), 
                                previous).
    
        var t1 := printing compiled.
    
        t1().           
    ]
}

public program
[
    Algorithm execute(2).
    Algorithm execute(2,3).
    Algorithm execute(2,3,4).
    Algorithm execute(2,3,4,5).
]