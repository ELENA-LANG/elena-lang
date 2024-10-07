//class A
//{
//    string s;
//    object  others;
//    
//    constructor new1(params int[] others)
//    {
//        this others := others;
//    }
//    
//    constructor new2(string s, params int[] others)
//    {
//        this s := s;
//        this others := others;
//    }
//}
//
//public program()
//{
//    var aa := A;
//    
//    var o1 := aa.new1(1);
//    var o2 := aa.new1(1,2); // raises method-not-found exception
//    var o3 := aa.new1(1,2,3);
//    var o4 := aa.new2("abc",1,2,3);
//}

//import system'dynamic'expressions;
//import extensions;
//
//public program()
//{
//    auto c := DynamicSingleton.load(
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
//    console.write(".")    
//}

A
{
   field x;

   constructor()
   {
      x := 3;
   }

   constructor new(arg)
   {
      x := arg
   }
}

public program()
{
   var a := A.new(2);
}