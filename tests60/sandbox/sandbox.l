import system'dynamic'expressions;

//import extensions;
//import system'routines'stex;
//import system'collections;
// 
//const int N = 20000;
// 
//extension op : IntNumber
//{
//    Enumerator<int> ProperDivisors
//        = new Range(1,self / 2).filterBy:(int n => self.mod:n == 0);
// 
//    get AmicablePairs()
//    {
//        auto divsums := new List<int>(
//            cast Enumerator<int>(
//                new Range(0, self).selectBy:(int i => i.ProperDivisors.summarize(0))));
// 
//        ^ new Range(0, divsums.Length)
//            .filterBy:(int i)
//            {
//                auto sum := divsums[i];
//                ^ (i < sum) && (sum < divsums.Length) && (divsums[sum] == i)
//            }
//            .selectBy:(int i => new Tuple<int,int>(i,divsums[i]));
//    }
//}
// 
//public program()
//{
//    N.AmicablePairs.forEach:(var Tuple<int,int> pair)
//    {
//        console.printLine(pair.Item1, " ", pair.Item2)
//    }
//}

public program()
{
                Expression.Method(
                   "eval",
                   Expression.CodeBlock(                   
                      Expression.If(                      
                          Expression.MessageCall(
                            new Message("notequal[2]"),
                            Expression.Variable("a"),
                            Expression.Variable("b")
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(true) 
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(false) 
                          )
                      )
                   ),
                   ScopeIdentifier.Variable("a"),
                   ScopeIdentifier.Variable("b")
                )

/*
    auto c := DynamicSingleton.new(
                Expression.Method(
                   "eval",
                   Expression.CodeBlock(                   
                      Expression.If(                      
                          Expression.MessageCall(
                            new Message("notequal[2]"),
                            Expression.Variable("a"),
                            Expression.Variable("b")
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(true) 
                          ),                             
                          Expression.CodeBlock(                          
                            Expression.Constant(false) 
                          )
                      )
                   ),
                   ScopeIdentifier.Variable("a"),
                   ScopeIdentifier.Variable("b")
                )
             );

    var o := c.compile();

    Assert.ifTrue(o.eval(1,2));
    Assert.ifFalse(o.eval(1,1));*/
}