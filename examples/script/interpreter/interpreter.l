import system'collections;
import system'routines;
import system'dynamic'expressions;

import extensions;
import extensions'scripting;

class TapeAssembler
{    
    Stack            theBrackets;
    List<Expression> theTape;
    
    constructor()
    {
        theBrackets := new Stack();
        theTape := new List<Expression>();
        
        theTape.append(Expression.DeclareAndAssigning(
            new ScopeVariable("ptr"),
            Expression.Constant(0)));
    }
    
    constructor load(assembly_program)
    {
        assembly_program(self)
    }    
    
    open()
    {
        theBrackets.push(theTape);
        theTape := new List<Expression>();
    }
    
    close()
    {
        var loop := Expression.Loop(
                          Expression.MessageCall(
                             new Message("notequal[2]"),
                             Expression.MessageCall(
                                new Message("at[2]"),
                                Expression.Variable("tape"),
                                Expression.Variable("ptr")
                             ),
                             Expression.Constant($0)
                          ),
                          Expression.CodeBlock(theTape.Value));
                          
        theTape := theBrackets.pop();
        theTape.append(loop)                 
    }
    
    input()
    {
        theTape.append(
            Expression.MessageCall(
                new Message("setAt[3]"),
                Expression.Variable(new ScopeVariable("tape")),
                Expression.Variable(new ScopeVariable("ptr")),
                Expression.MessageCall(
                    new Message("readChar[1]"),
                    Expression.Constant(console)
                )
            )
        )
    }
    
    output()
    {
        theTape.append(
            Expression.MessageCall(
                 new Message("write[2]"),
                 Expression.Constant(console), 
                 Expression.MessageCall(
                   new Message("at[2]"),
                   Expression.Variable(new ScopeVariable("tape")),
                   Expression.Variable(new ScopeVariable("ptr"))
                 )
            )
        )
    }
    
    next()
    {
        theTape.append(
            Expression.Assigning(
                new ScopeVariable("ptr"),
                Expression.MessageCall(
                    new Message("add[2]"),
                    Expression.Variable(new ScopeVariable("ptr")),
                    Expression.Constant(1))))
    }
    
    previous()
    {
        theTape.append(
            Expression.Assigning(
                new ScopeVariable("ptr"),
                Expression.MessageCall(
                    new Message("subtract[2]"),
                    Expression.Variable(new ScopeVariable("ptr")),
                    Expression.Constant(1))))
    }
    
    increase()
    {
        theTape.append(
            Expression.MessageCall(
                new Message("setAt[3]"),            
                Expression.Variable("tape"),
                Expression.Variable("ptr"), 
                Expression.MessageCall(
                    new Message("load[2]"),                    
                    Expression.Constant(CharValue),
                    Expression.MessageCall(
                        new Message("add[2]"),
                        Expression.MessageCall(                                    
                            new Message("toInt[2]"),                           
                            Expression.Constant(convertor),
                            Expression.MessageCall(
                                new Message("at[2]"),
                                Expression.Variable("tape"),
                                Expression.Variable("ptr")
                            )
                        ),
                        Expression.Constant(1)
                    )
                )
            ));
    }
    
    decrease()
    {
        theTape.append(
            Expression.MessageCall(
                new Message("setAt[3]"),            
                Expression.Variable("tape"),
                Expression.Variable("ptr"), 
                Expression.MessageCall(
                    new Message("load[2]"),                    
                    Expression.Constant(CharValue),
                    Expression.MessageCall(
                        new Message("subtract[2]"),
                        Expression.MessageCall(                                    
                            new Message("toInt[2]"),                           
                            Expression.Constant(convertor),
                            Expression.MessageCall(
                                new Message("at[2]"),
                                Expression.Variable("tape"),
                                Expression.Variable("ptr")
                            )
                        ),
                        Expression.Constant(1)
                    )
                )
            ));
    }
    
    compile()
    {
        var program := DynamicSingleton.new(
                    Expression.Method(
                       "eval",
                       new ScopeVariable("tape"),                   
                       Expression.CodeBlock(theTape.Value))).compile(nil, IdleExpressionScope);           
                   
        ^(tape){ program.eval(tape) }
    }
}

class TapeCGenerator
{    
    object theOutput;
    
    constructor()
    {
        theOutput := new system'text'TextBuilder()
    }

    constructor load(assembly_program)
    {
        assembly_program(self)
    }    
    
    open()
    {
        theOutput.printLine:"while (*p) {"
    }

    close()
    {
        theOutput.printLine:"}"
    }
    
    input()
    {
        theOutput.printLine:"*p = getchar();"
    }
    
    output()
    {
        theOutput.printLine:"putchar(*p);"
    }
    
    next()
    {
        theOutput.printLine:"++p;"
    }
    
    previous()
    {
        theOutput.printLine:"--p;"
    }

    increase()
    {
        theOutput.printLine:"++*p;"
    }
    
    decrease()
    {
        theOutput.printLine:"--*p;"
    }
    
    compile()
    {
        theOutput.insert(0,"void execute(char* p) {");
        theOutput.printLine:"}";
        
        ^ theOutput.Value
    }
}

public solution1()
{
    // .[,.]
    
    auto c := DynamicSingleton.new(
                Expression.Method(
                   "eval",
                   new ScopeVariable("tape"),                   
                   Expression.CodeBlock(                   
                      // var ptr := 0
                      Expression.DeclareAndAssigning(
                        new ScopeVariable("ptr"),
                        Expression.Constant(0)
                      ),
                      // tape[ptr] := console.readChar()
                      Expression.MessageCall(
                        new Message("setAt[3]"),
                        Expression.Variable(new ScopeVariable("tape")),
                        Expression.Variable(new ScopeVariable("ptr")),
                        Expression.MessageCall(
                           new Message("readChar[1]"),
                           Expression.Constant(console)
                        )
                      ),
                      // while (tape[ptr]!=0) {
                      Expression.Loop(                      
                          Expression.MessageCall(
                            new Message("notequal[2]"),
                             Expression.MessageCall(
                                new Message("at[2]"),
                                Expression.Variable(new ScopeVariable("tape")),
                                Expression.Variable(new ScopeVariable("ptr"))
                             ),
                             Expression.Constant($0)
                          ),                             
                          Expression.CodeBlock(                          
                              // console.write(tape[ptr])  
                              Expression.MessageCall(
                                 new Message("write[2]"),
                                 Expression.Constant(console), 
                                 Expression.MessageCall(
                                   new Message("at[2]"),
                                   Expression.Variable(new ScopeVariable("tape")),
                                   Expression.Variable(new ScopeVariable("ptr"))
                                 )
                              ),                                 
                              // tape[ptr] := console readChar
                              Expression.MessageCall(
                                new Message("setAt[3]"),
                                Expression.Variable(new ScopeVariable("tape")),
                                Expression.Variable(new ScopeVariable("ptr")), 
                                Expression.MessageCall(
                                    new Message("readChar[1]"),
                                    Expression.Constant(console)
                                )
                              )
                          )
                      )
                   )
                )
             );

    var bf_tape := Array.allocate(1024).populate:(n => $0);
    var o := c.compile();

    o.eval(bf_tape)
}    

public solution2()
{
    var bfProgram := new TapeAssembler()
        .input().open().output().input().close().compile();
                    
    var bfTape := Array.allocate(1024).populate:(n => $0);
    
    bfProgram(bfTape)
}

public solution3()
{
    var bfAssemblyProgram := new ScriptEngine() 
        .loadPath("asmrules.es")
        .buildScript(">,[.,]");

    var bfProgram := TapeAssembler.load(bfAssemblyProgram).compile();

    var bfTape := Array.allocate(1024).populate:(n => $0);

    bfProgram(bfTape)
}

public solution4()
{
    if (program_arguments.Length == 1)
        { console.write:"Please provide the path to the file to interpret"; AbortException.raise() };

    var bfAssemblyProgram := new ScriptEngine() 
        .loadPath("asmrules.es")
        .buildScriptFile(program_arguments[1]);

    var bfProgram := TapeAssembler.load(bfAssemblyProgram).compile();

    var bfTape := Array.allocate(1024).populate:(n => $0);

    bfProgram(bfTape)
}

public solution5()
{
    if (program_arguments.Length == 1)
        { console.write:"Please provide the path to the file to interpret"; AbortException.raise() };

    var bfAssemblyProgram := new ScriptEngine() 
        .loadPath("asmrules.es")
        .buildScriptFile(program_arguments[1]);

    var bfProgram := TapeCGenerator.load(bfAssemblyProgram).compile();

    console.writeLine(bfProgram)
}

const int Solution = 1;

public program()
{
    Solution =>
        1 {
            solution1()
        }
        2 {
            solution2()
        }
        3 {
            solution3()
        }
        4 {
            solution4()
        }
        5 {
            solution5()
        };
}