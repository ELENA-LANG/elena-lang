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
   IntTester.test(i * 2, 6)
}
