import extensions;
//import system'math;
//
//public extension lazyOp
//{
//   whileFalse(Func action)
//   {
//      until(self)
//      {
//         action()
//      }
//   }        
//}
//
//public program()
//{
//    var n := 1;
//    
//    (:lazy (n.sqr().mod(100) == 64)).whileFalse({ n := n + 1 });
//    
//    console.printLine(n)
//}

//#new __symbol aliases;

//#let aliases["myint"]  := system'IntNumber;

//#load sandbox'aliases;

//myint X = 2;

public program()
{
    real pi := 0.0;
/*    for(*/int i := 0//; i < 1; i += 1)
//    {
        pi += -1.0/*.power(i)*/ / (i/**2*/+1)/* * 4*/;
//    };

    console.printLine(pi)
}