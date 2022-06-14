import system;

A;

singleton Tester
{
   run()
   {
      ifnot(false) {
         console.writeLine("ifnot works")
      };

      ifnot(true) {
         console.writeLine("ifnot doesn't works")
      };

      for (int n := 0, n < 5, n := n + 1) {
         console.write("*");
      }
   }
}

public symbol program = Tester.run();
