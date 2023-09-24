import system'collections;
import system'routines;
import system'dynamic'expressions;

import extensions;
import extensions'scripting;

const bf_program = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";

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
                Expression.Constant(console)
             )
            )
        )
    }
    
    output()
    {
        _tape.append(
            Expression.MessageCall(
                 new Message("write[2]"),
                 Expression.Constant(console), 
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
    
    compiled()
    {
        var program := DynamicSingleton.load(
                    Expression.Method(
                       "eval",
                       CodeblockExpression.new(_tape.Value),
                       ScopeIdentifier.Variable("tape"))
                    ).compile();           
                   
        ^(tape){ program.eval(tape) }
    }
}

public program()
{
    var bfAssemblyProgram := new ScriptEngine() 
        .loadPath("asmrules.es")
        .buildScript(bf_program);

    var bfProgram := TapeAssembler.load(bfAssemblyProgram).compiled();

    var bfTape := Array.allocate(1024).populate:(n => $0);

    console.writeLine(bf_program);
    bfProgram(bfTape)
}
