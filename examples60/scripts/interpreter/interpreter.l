import system'collections;
import system'routines;
import system'dynamic'expressions;

import extensions;
import extensions'scripting;

class TapeAssembler
{    
    Stack            _brackets;
    List<Expression> _tape;
    
    constructor()
    {
        _brackets := new Stack();
        _tape := new List<Expression>();
        
        _tape.append(Expression.DeclareAndAssigning(
            "ptr",
            Expression.Constant(0)));
    }
    
    constructor load(assembly_program)
    {
        assembly_program(self)
    }    
    
    open()
    {
        _brackets.push(_tape);
        _tape := new List<Expression>();
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
                          CodeblockExpression.new(_tape.Value));
                          
        _tape := _brackets.pop();
        _tape.append(loop)                 
    }
    
    input()
    {
        _tape.append(
            Expression.MessageCall(
                new Message("setAt[3]"),
                Expression.Variable("tape"),
                Expression.Variable("ptr"),
                Expression.MessageCall(
                    new Message("readChar[1]"),
                    Expression.Constant(Console)
                )
            )
        )
    }
    
    output()
    {
        _tape.append(
            Expression.MessageCall(
                 new Message("write[2]"),
                 Expression.Constant(Console), 
                 Expression.MessageCall(
                   new Message("at[2]"),
                   Expression.Variable("tape"),
                   Expression.Variable("ptr")
                 )
            )
        )
    }
    
    next()
    {
        _tape.append(
            Expression.Assigning(
                "ptr",
                Expression.MessageCall(
                    new Message("add[2]"),
                    Expression.Variable("ptr"),
                    Expression.Constant(1))))
    }
    
    previous()
    {
        _tape.append(
            Expression.Assigning(
                "ptr",
                Expression.MessageCall(
                    new Message("subtract[2]"),
                    Expression.Variable("ptr"),
                    Expression.Constant(1))))
    }
    
    increase()
    {
        _tape.append(
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
                            Expression.Constant(intConvertExt),
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
        _tape.append(
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
                            Expression.Constant(intConvertExt),
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
        var program := DynamicSingleton.load(
                    Expression.Method(
                       "eval",
                       CodeblockExpression.new(_tape.Value),
                       ScopeIdentifier.Variable("tape"))).compile(nil, IdleExpressionScope);           
                   
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
        theOutput.printLine("while (*p) {")
    }

    close()
    {
        theOutput.printLine("}")
    }
    
    input()
    {
        theOutput.printLine("*p = getchar();")
    }
    
    output()
    {
        theOutput.printLine("putchar(*p);")
    }
    
    next()
    {
        theOutput.printLine("++p;")
    }
    
    previous()
    {
        theOutput.printLine("--p;")
    }

    increase()
    {
        theOutput.printLine("++*p;")
    }
    
    decrease()
    {
        theOutput.printLine("--*p;")
    }
    
    compile()
    {
        theOutput.insert(0,"void execute(char* p) {");
        theOutput.printLine("}");
        
        ^ theOutput.Value
    }
}

public solution0()
{
    // .[,.]
    
    auto c := DynamicSingleton.load(
                Expression.Method(
                   "eval",
                   Expression.CodeBlock(                   
                      // var ptr := 0
                      Expression.DeclareAndAssigning(
                        "ptr",
                        Expression.Constant(0)
                      ),
                      // tape[ptr] := Console.readChar()
                      Expression.MessageCall(
                        new Message("setAt[3]"),
                        Expression.Variable("tape"),
                        Expression.Variable("ptr"),
                        Expression.MessageCall(
                           new Message("readChar[1]"),
                           Expression.Constant(Console)
                        )
                      ),
                      // while (tape[ptr]!=0) {
                      Expression.Loop(                      
                          Expression.MessageCall(
                            new Message("notequal[2]"),
                             Expression.MessageCall(
                                new Message("at[2]"),
                                Expression.Variable("tape"),
                                Expression.Variable("ptr")
                             ),
                             Expression.Constant($0)
                          ),                             
                          Expression.CodeBlock(                          
                              // Console.write(tape[ptr])  
                              Expression.MessageCall(
                                 new Message("write[2]"),
                                 Expression.Constant(Console), 
                                 Expression.MessageCall(
                                   new Message("at[2]"),
                                   Expression.Variable("tape"),
                                   Expression.Variable("ptr")
                                 )
                              ),                                 
                              // tape[ptr] := Console readChar
                              Expression.MessageCall(
                                new Message("setAt[3]"),
                                Expression.Variable("tape"),
                                Expression.Variable("ptr"), 
                                Expression.MessageCall(
                                    new Message("readChar[1]"),
                                    Expression.Constant(Console)
                                )
                              )
                          )
                      )
                   ),
                   ScopeIdentifier.Variable("tape")
                )
             );

    var bf_tape := Array.allocate(1024).populate::(n => $0);
    var o := c.compile();

    o.eval(bf_tape)
}    

public solution1()
{
    var bfProgram := new TapeAssembler()
        .input().open().output().input().close().compile();
                    
    var bfTape := Array.allocate(1024).populate::(n => $0);
    
    bfProgram(bfTape)
}

public solution2()
{
    var bfAssemblyProgram := new ScriptEngine() 
        .loadPath("asmrules.es")
        .buildScript(">,[.,]");

    var bfProgram := TapeAssembler.load(bfAssemblyProgram).compile();

    var bfTape := Array.allocate(1024).populate::(n => $0);

    bfProgram(bfTape)
}

public solution3(string path)
{
    var bfAssemblyProgram := new ScriptEngine() 
        .loadPath("asmrules.es")
        .buildScriptFile(path);

    var bfProgram := TapeAssembler.load(bfAssemblyProgram).compile();

    var bfTape := Array.allocate(1024).populate::(n => $0);

    bfProgram(bfTape)
}

public solution4(string path)
{
    var bfAssemblyProgram := new ScriptEngine() 
        .loadPath("asmrules.es")
        .buildScriptFile(path);

    var bfProgram := TapeCGenerator.load(bfAssemblyProgram).compile();

    Console.writeLine(bfProgram)
}

public program()
{
   int solution := 3;

   if (program_arguments.Length == 1)
   { 
      Console.write("Please provide the path to the file to interpret"); AbortException.raise() 
   }
   else if (program_arguments.Length == 3) {
      solution := program_arguments[2].toInt();
   };

   string path := program_arguments[1];

   solution =>
      0 : {
         solution0()
      }
      1 : {
         solution1()
      }
      2 : {
         solution2()
      }
      3 : {
         solution3(path)
      }
      ! : {
         solution4(path)
      };
}