import system;

B;

A
{
   B cast()
      = new B();
}


singleton Tester
{
   run()
   {
      A a := new A();
      B b := a;

//       byte b := 55;

//       byte tmp[10];
//       tmp[0] := b;
//       tmp[1] := 57;
//       tmp[2] := 0;


//      for (int n := 0, n < 5, n := n + 1) {
//         console.write("*");
//      }
   }
}

public symbol program = Tester.run();
