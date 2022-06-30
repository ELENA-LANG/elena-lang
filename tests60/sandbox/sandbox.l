import system;

singleton Tester
{
   run()
   {
       byte b := 55;
       byte z := 0;

       byte tmp[10];
       tmp[0] := b;
//       b := tmp[0];
       tmp[1] := b;
       tmp[2] := b;
       tmp[3] := z;


//      for (int n := 0, n < 5, n := n + 1) {
//         console.write("*");
//      }
   }
}

public symbol program = Tester.run();
