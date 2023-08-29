import extensions;
//
//class MyClass
//{
//    doInit()
//    {
//        console.writeLine:"onInit fired!"
//    }
//}
//
//class MainClass
//{
//    event Func onInit; // a field with the same name is declared as well
//
//    init()
//    {
//        // call the function if it is not nil
//        onInit.?()
//    }    
//}
//
//public program()
//{
//    var class := new MyClass();
//
//    var mainClass := new MainClass();
//
//    // adding an event handler 
//    mainClass.onInit := { class.doInit() };
//    // adding the second event handler 
//    mainClass.onInit := { console.writeLine:"I'm fired too!!" }; 
//
//    mainClass.init()
//}

//public program()
//{
//   var f := /*{ console.writeLine("Hello") }*/nil;
//
//   f();
//}

struct XY
{
   int x : prop;
   int y : prop;

   constructor()
   {
      x := 0;
      y := 0;
   }

   set(int x, int y)
   {
      this x := x;
      this y := y;
   }

   set(long r, long r2) {}

   printMe()
   {
      console.printLine("x:", x);
      console.printLine("y:", y);
   }
}

singleton Tester
{
//   testMe(XY r)
//   {
//      r.set(1,2);
//   }
   testMe2(XY r)
   {
      var a := 3;
      var b := 4;

      r.set(a,b);
   }
}

public program()
{
   XY xy := new XY();
   var xy2 := new XY();

   Tester.testMe2(xy);
   Tester.testMe2(xy2);

   xy.printMe();
   xy2.printMe();
}
