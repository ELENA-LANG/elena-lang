import extensions;
//import system'dynamic;
////import system'dynamic'expressions;
//
//public singleton MyDynamicallyLoadedClass
//{
//    doSomething()
//    {
//        console.writeLine("did something")
//    }
//}
//
//symbol used : preloaded = new object[]{MyDynamicallyLoadedClass, mssg doSomething};
//    
//dynamicEvaluating()
//{
//    var loadedClass := new Symbol("sandbox'MyDynamicallyLoadedClass");
//    var loadedMessage := new Message("doSomething[1]");
//    
//    var t := DynamicTape.load(
//        ConstantFunction.load(loadedClass),
//        MessageFunction.load(loadedMessage));
//
//    t()
//}    
//    
////treeEvaluating()
////{
////    var loadedMessage := new Message("doSomething[1]");
//    
////    var c := Expression.MessageCall(
////                            loadedMessage, 
////                            Expression.Symbol("exprtree1'MyDynamicallyLoadedClass"));
//                            
////    c.eval()                                
////}    

//class A
//{
//   int l;
//
//   constructor new(params int[] args)
//   {
//      l := args.Length;
//   }
//}

//public program()
//{
//   var n := 3;
//
//   var a := A.new(1,2,n);

//    dynamicEvaluating();
//    
////    treeEvaluating()
//}


singleton VariadicFunctionDispatchTester
{
   function(params int[] args)
   {
      int l := args.Length;
      int sum := 0;
      for (int i := 0, i < l, i += 1) {
         sum := sum + args[i]
      };

      ^ sum
   }
}

singleton VariadicDispatchTester
{
   product(params int[] args)
   {
      int l := args.Length;
      int prod := 1;
      for (int i := 0, i < l, i += 1) {
         prod := prod * args[i]
      };

      ^ prod
   }
}

public program()
{
//   console.write("vardispatcherTest:");

   var o2 := VariadicDispatchTester;
   var r4 := o2.product(1,2,3,4);
   Assert.ifTrue(r4 == 24);

   console.writeLine(".");
}
