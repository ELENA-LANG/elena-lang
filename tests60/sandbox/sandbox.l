import extensions;

public program()
{
    auto c := DynamicSingleton.load(
                Expression.Method(
                   "eval",
                   Expression.CodeBlock(                   
                      Expression.If(                      
                          Expression.MessageCall(
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