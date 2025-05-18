import extensions;
import system'dynamic'expressions;

public program()
{
    auto c := DynamicSingleton.load(
                Expression.Method(
                   "eval",
                   Expression.CodeBlock(                   
                      Expression.If(                      
                          CondMessageCallExpression.load(
                            new Message("equal[2]"),
                            Expression.Variable("a"),
                            Expression.Identifier("nil")
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(true) 
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(false) 
                          )
                      )
                   ),
                   ScopeIdentifier.Variable("a")
                )
             );

    var o := c.compile();

    Assert.ifTrue(o.eval(nil));

    Console.write(".")   
}