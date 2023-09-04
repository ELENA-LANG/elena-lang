//import extensions;
//import system'dynamic'expressions;
// 
//public program()
//{
//    console.write("ifExpressionTest:");
//    
//    auto c := DynamicSingleton.new(
//                Expression.Method(
//                   "eval",
//                   Expression.CodeBlock(                   
//                      Expression.If(                      
//                          Expression.MessageCall(
//                            new Message("notequal[2]"),
//                            Expression.Variable("a"),
//                            Expression.Variable("b")
//                          ),                             
//                          Expression.CodeBlock(                          
//                            Expression.Constant(true) 
//                          ),                             
//                          Expression.CodeBlock(                          
//                            Expression.Constant(false) 
//                          )
//                      )
//                   ),
//                   ScopeIdentifier.Variable("a"),
//                   ScopeIdentifier.Variable("b")
//                )
//             );
//
//    var o := c.compile();
//
//    Assert.ifTrue(o.eval(1,2));
//    Assert.ifFalse(o.eval(1,1));
//
//    console.writeLine(".")    
//}

import system'collections;

A
{
   Reference<int> r;

   set(int n)
   {
      r := n;
   }
}

public program()
{
   A a := new A();
   a.set(2);

//   List<object> list := new List<object>(1);
//   list.append("abc");
//   list.append("def");
}