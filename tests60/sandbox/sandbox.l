import system;

singleton Tester
{
   run()
   {
       byte n;
       byteConvertor.convert(2, ref n);

//       byte b := 0;
//       byteConvertor.convert(ref b);


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
