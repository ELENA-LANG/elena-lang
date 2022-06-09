import system;

//A
//{
//   constructor()
//   {
//      console.writeLine("new A()")
//   }
//}

//static a = new A();

singleton Tester
{
   run()
   {
      console.writeLine("Line1");
      console.writeLine("Line2");
      console.writeLine("Line3");

//      var a1 := a;
//      var a2 := a;

//      for (int n := 0, n < 5, n := n + 1) {
//         console.write("*");
//      }
   }
}

public symbol program = Tester.run();
