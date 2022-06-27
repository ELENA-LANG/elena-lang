import system;

public singleton byteConvertor
{
   convert(/*int n, */ref byte retVal)
   {
      byte b := 2;

//      retVal := 2;
   }
}

singleton Tester
{
   run()
   {
       byte b := 0;
       byteConvertor.convert(ref b);


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
