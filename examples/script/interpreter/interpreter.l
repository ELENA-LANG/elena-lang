//import system'collections;
import system'routines;
import system'dynamic'expressions;
//
//import extensions;
//import extensions'scripting;
////import extensions'dynamic;
//import extensions'dynamic'expressions;
//
//class TapeAssembler
//{    
//    Stack                theBrackets;
//    List<TapeExpression> theTape;
//    
//    constructor()
//    {
//        theBrackets := new Stack();
//        theTape := new List<TapeExpression>();
//        
//        theTape.append(TapeExpression.Declaring("ptr"));
//        theTape.append(TapeExpression.Assigning("ptr", TapeExpression.Constant(0)));
//    }
//    
//    constructor load(assembly_program)
//        <= ()
//    {
//        assembly_program(self)
//    }    
//    
//    open()
//    {
//        theBrackets.push(theTape);
//        theTape := new List<TapeExpression>();
//    }
//    
//    close()
//    {
//        var loop := TapeExpression.Loop(
//                          TapeExpression.MessageCall(
//                             TapeExpression.MessageCall(
//                                TapeExpression.Variable("tape"),
//                                "at",
//                                TapeExpression.Variable("ptr")
//                             ),
//                             "notequal",
//                             TapeExpression.Constant($0)),
//                          TapeExpression.Code(theTape.Value));
//                          
//        theTape := theBrackets.pop();
//        theTape.append(loop)                 
//    }
//    
//    input()
//    {
//        theTape.append(TapeExpression.MessageCall(
//                        TapeExpression.Variable("tape"),
//                        "setAt",
//                        TapeExpression.Variable("ptr"),
//                        TapeExpression.MessageCall(
//                           TapeExpression.Constant(console),
//                           "readChar"
//                        )))
//    }
//    
//    output()
//    {
//        theTape.append(TapeExpression.MessageCall(
//                                 TapeExpression.Constant(console), 
//                                 "write",
//                                 TapeExpression.MessageCall(
//                                   TapeExpression.Variable("tape"),
//                                   "at",
//                                   TapeExpression.Variable("ptr")
//                                 )))
//    }
//    
//    next()
//    {
//        theTape.append(TapeExpression.Assigning(
//            "ptr",
//            TapeExpression.MessageCall(
//                TapeExpression.Variable("ptr"),
//                "add",
//                TapeExpression.Constant(1))))
//    }
//    
//    previous()
//    {
//        theTape.append(TapeExpression.Assigning(
//            "ptr",
//            TapeExpression.MessageCall(
//                TapeExpression.Variable("ptr"),
//                "subtract",
//                TapeExpression.Constant(1))))
//    }
//    
//    increase()
//    {
//        theTape.append(TapeExpression.MessageCall(
//                                TapeExpression.Variable("tape"),
//                                "setAt",
//                                TapeExpression.Variable("ptr"), 
//                                TapeExpression.MessageCall(
//                                    TapeExpression.Constant(CharValue),
//                                    "load",
//                                    TapeExpression.MessageCall(
//                                        TapeExpression.MessageCall(
//                                            TapeExpression.Constant(convertor),
//                                            "toInt",
//                                            TapeExpression.MessageCall(
//                                                TapeExpression.Variable("tape"),
//                                                "at",
//                                                TapeExpression.Variable("ptr"))
//                                        ),
//                                        "add",
//                                        TapeExpression.Constant(1)))))
//    }
//    
//    decrease()
//    {
//        theTape.append(TapeExpression.MessageCall(
//                                TapeExpression.Variable("tape"),
//                                "setAt",
//                                TapeExpression.Variable("ptr"), 
//                                TapeExpression.MessageCall(
//                                    TapeExpression.Constant(CharValue),
//                                    "load",
//                                    TapeExpression.MessageCall(
//                                        TapeExpression.MessageCall(
//                                            TapeExpression.Constant(convertor),
//                                            "toInt",
//                                            TapeExpression.MessageCall(
//                                                TapeExpression.Variable("tape"),
//                                                "at",
//                                                TapeExpression.Variable("ptr"))
//                                        ),
//                                        "subtract",
//                                        TapeExpression.Constant(1)))))
//
//    }
//    
//    get()
//    {
//        var program := TapeExpression.Singleton(
//                TapeExpression.Method(
//                   "eval",
//                   TapeExpression.Code(theTape.Value),
//                   TapeExpression.Parameter("tape")));
//                   
//        var o := (program.compiled())();
//                   
//        ^(tape){ o.eval(tape) }
//    }
//}
//
////class TapeCGenerator
////{    
////    object theOutput.
////    
////    constructor new
////    [
////        theOutput := system'text'TextBuilder new.
////    ]
////    
////    constructor new : assembly_program
////        <= new;
////    [
////        assembly_program(self).
////    ]    
////    
////    open
////    [
////        theOutput printLine:"while (*p) {".
////    ]
////
////    close
////    [
////        theOutput printLine:"}".
////    ]
////    
////    input
////    [
////        theOutput printLine:"*p = getchar();".
////    ]
////    
////    output
////    [
////        theOutput printLine:"putchar(*p);".
////    ]
////    
////    next
////    [
////        theOutput printLine:"++p;".
////    ]
////    
////    previous
////    [
////        theOutput printLine:"--p;".
////    ]
////
////    increase
////    [
////        theOutput printLine:"++*p;".
////    ]
////    
////    decrease
////    [
////        theOutput printLine:"--*p;".
////    ]
////    
////    get
////    [
////        theOutput insert:"void execute(char* p) {" at:0.
////        theOutput printLine:"}".
////        
////        ^ theOutput literal.
////    ]
////}

public solution1()
{
    // .[,.]
    
    var c := DynamicSingleton.new(
                Expression.Method(
                   "eval",
                   new ScopeVariable("tape"),                   
                   Expression.CodeBlock(                   
                      Expression.Declaration(new ScopeVariable("ptr")),                      
                      // ptr := 0
                      Expression.Assigning(
                        new ScopeVariable("ptr"),
                        Expression.Constant(0)
                      ),
                      // tape[ptr] := console.readChar()
                      Expression.MessageCall(
                        new Message("setAt[2]"),
                        Expression.Variable(new ScopeVariable("tape")),
                        Expression.Variable(new ScopeVariable("ptr")),
                        Expression.MessageCall(
                           new Message("readChar[0]"),
                           Expression.Constant(console)
                        )
                      ),
                      // while (tape[ptr]!=0) {
                      Expression.Loop(                      
                          Expression.MessageCall(
                            new Message("notequal[1]"),
                             Expression.MessageCall(
                                new Message("at[1]"),
                                Expression.Variable(new ScopeVariable("tape")),
                                Expression.Variable(new ScopeVariable("ptr"))
                             ),
                             Expression.Constant($0)
                          ),                             
                          Expression.CodeBlock(                          
                              // console.write(tape[ptr])  
                              Expression.MessageCall(
                                 new Message("write[1]"),
                                 Expression.Constant(console), 
                                 Expression.MessageCall(
                                   new Message("at[1]"),
                                   Expression.Variable(new ScopeVariable("tape")),
                                   Expression.Variable(new ScopeVariable("ptr"))
                                 )
                              ),                                 
                              // tape[ptr] := console readChar
                              Expression.MessageCall(
                                new Message("setAt[2]"),
                                Expression.Variable(new ScopeVariable("tape")),
                                Expression.Variable(new ScopeVariable("ptr")), 
                                Expression.MessageCall(
                                    new Message("readChar[0]"),
                                    Expression.Constant(console)
                                )
                              )
                          )
                      )
                   )
                )
             );

    var bf_tape := Array.allocate(1024).populate:(n => $0);
    var o := c.compiled();

    o.eval(bf_tape)
}    

//public program2()
//{
//    var bfProgram := new TapeAssembler()
//        .input().open().output().input().close().get();
//                    
//    var bfTape := Array.allocate(1024).populate:(n => $0);
//    
//    bfProgram(bfTape)
//}
//
//public program3bis()
//{
//    var bfAssemblyProgram := new ScriptEngine() 
//        .loadPath("asmrules.es")
//        .eval(">,[.,]");
//
//    var bfProgram := TapeAssembler.load(bfAssemblyProgram).get();
//
//    var bfTape := Array.allocate(1024).populate:(n => $0);
//
//    bfProgram(bfTape)
//}
//
//public program3()
//{
//    if (program_arguments.Length == 1)
//        { console.write:"Please provide the path to the file to interpret"; AbortException.raise() };
//
//    var bfAssemblyProgram := new ScriptEngine() 
//        .loadPath("asmrules.es")
//        .evalPath(program_arguments[1]);
//
//    var bfProgram := TapeAssembler.load(bfAssemblyProgram).get();
//
//    var bfTape := Array.allocate(1024).populate:(n => $0);
//
//    bfProgram(bfTape)
//}
//
////public symbol program4 =
////[
////    if (forwards'program'arguments length == 1)
////        [ console write:"Please provide the path to the file to interpret". AbortException new; raise ].
////
////    var bfAssemblyProgram := ScriptEngine 
////        new;
////        loadPath:"asmrules.es";
////        evalPath:(forwards'program'arguments[1]).
////
////    var bfProgram := TapeCGenerator new(bfAssemblyProgram); get.
////
////    console writeLine:bfProgram.
////].

public program()
{
    solution1();
}