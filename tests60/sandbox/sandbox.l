import extensions;

const int N = 3;

singleton IntTester
{
   test(int val, int checkVal)
   {
      Assert.ifTrue(val == checkVal)
   }

   testWeak(val, checkVal)
   {
      Assert.ifTrue(val == checkVal)
   }
}

public program()
{
   int i := 3;
//   IntTester.test(i * N, 9);

//   IntTester.testWeak(i * N, 9);

//   if (i == N) {
//      console.writeLine("!!");
//   };

//   int j := N;
   int k := i + N;
}
