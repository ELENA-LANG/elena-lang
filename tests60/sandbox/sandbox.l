import extensions;
import extensions'scripting;
import system'dynamic'expressions;

//public program()
//{
//    auto c := DynamicSingleton.load(
//                Expression.Method(
//                   "eval",
//                   Expression.CodeBlock(                   
//                      Expression.If(                      
//                          new CondMessageCallExpression(
//                            Expression.Variable("a"),
//                            "equal[2]",
//                            Expression.Identifier("nil")
//                          ),                             
//                          Expression.CodeBlock(                          
//                            Expression.Constant(true) 
//                          ),                             
//                          Expression.CodeBlock(                          
//                            Expression.Constant(false) 
//                          )
//                      )
//                   ),
//                   ScopeIdentifier.Variable("a")
//                )
//             );
//
//    var o := c.compile();
//
//    Assert.ifTrue(o.eval(nil));
//    Assert.ifTrue(o.eval(nilValue));
//    Assert.ifFalse(o.eval(2));
//
//    Console.write(".")   
//}

public program()
{
   var f := LScript.eval("
      public program()
      {
         var o := nil;
         if (o == nil) {
            system'Console.writeLine(""Done"");
         };
      }
").program;   

   f();
}
