import system'collections;
import system'routines;
import system'dynamic'expressions;

import extensions;
import extensions'scripting;

const bf_program = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";

class TapeAssembler
{    
    Stack            theBrackets;
    List<Expression> theTape;
    
    constructor()
    {
        theBrackets := new Stack();
        theTape := new List<Expression>();
        
        theTape.append(Expression.DeclareAndAssigning(
            "ptr",
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
        theTape.append(
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
        theTape.append(
            Expression.Assigning(
                "ptr",
                Expression.MessageCall(
                    new Message("add[2]"),
                    Expression.Variable("ptr"),
                    Expression.Constant(1))))
    }
    
    previous()
    {
        theTape.append(
            Expression.Assigning(
                "ptr",
                Expression.MessageCall(
                    new Message("subtract[2]"),
                    Expression.Variable("ptr"),
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
    
    compiled()
    {
        var program := DynamicSingleton.new(
                    Expression.Method(
                       "eval",
                       "tape",                   
                       Expression.CodeBlock(theTape.Value))).compiled();           
                   
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

    bfProgram(bfTape)
}
