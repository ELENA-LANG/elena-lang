import system;

A;

singleton Tester
{
   run()
   {
      var o := new Object();
      console.writeLine(o);

      var a := new A();
      console.writeLine(a);

      console.writeLine("*");

//      for (int n := 0, n < 5, n := n + 1) {
//         console.write("*");
//      }
   }
}

public symbol program = Tester.run();
