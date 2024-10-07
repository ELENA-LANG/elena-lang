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
import extensions;

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

namespace variadicBoxingTestset
{
   A
   {
      field args;

      constructor load(params string[] args)
      {
         this args := args;
      }

      int Length
         = args.Length;

      validate(params string[] args)
      {
         for (int i := 0; i < args.Length; i++) {
            Assert.ifEqual(this args[i], args[i]);
         }
      }

      static validate2(params string[] args)
      {
         string[] boxedArgs := args;

         Assert.ifEqual(boxedArgs.Length, args.Length);

         for (int i := 0; i < args.Length; i++) {
            Assert.ifEqual(boxedArgs[i], args[i]);
         }
      }
   }
}

public variadicBoxingTest()
{
   auto a := variadicBoxingTestset'A.load("a", "b", "c");
   
   Assert.ifEqual(a.Length, 3);
   a.validate("a", "b", "c");
   Console.write(".");

   variadicBoxingTestset'A.validate2("a", "b", "c");
   Console.write(".");
}

public program()
{
   variadicBoxingTest()
}