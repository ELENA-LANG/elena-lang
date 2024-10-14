import extensions;

//interface IA
//{
//   abstract bar();    
//}
//
//interface IB
//{
//    abstract foo();    
//}
//
//class A : IA
//{
//   bar()
//   {
//      Console.writeLine("bar")
//   }    
//}
//
//class B : IB
//{
//   foo()
//   {
//      Console.writeLine("foo")
//   }    
//}
//
//singleton C
//{
//   proceed(IA a)
//   {
//      Console.print(a,":");
//      a.bar()
//   }
//    
//   proceed(IB b)
//   {
//      Console.print(b,":");
//      b.foo()
//   }
//
//   // default handler
//   proceed(o)
//   {
//      Console.printLine(o,": unknown");
//   }    
//}
//
//Decorator
//{
//   field target;
//    
//   constructor(target)
//   {
//      this target := target
//   }
//    
//   generic()
//   {
//      Console.print("decorating ");
//        
//      __received(target)
//   }
//}
//
// public program()
// {
//    var a := new A();
////    var b := new B();
//    var decA := new Decorator(a);
////    var decB := new Decorator(b);
//
//    decA.foo();
////    decB.bar();
//}

public program()
{
   var n := RealNumber.Pi / 2;
   var f := mssg sin<system'math'mathOp>[0];
   
   console.printLine(f(n));
}
