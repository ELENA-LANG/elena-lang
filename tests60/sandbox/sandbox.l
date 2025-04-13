sealed class VariadicTester
{
   int sum(params int[] numbers)
   {
      int sum := 0;
      for (int i := 0; i < numbers.Length; i += 1) {
         sum += numbers[i];
      };

      ^ sum;
   }

   int passAndSum(params int[] numbers)
      <= sum(params numbers);
}

public variadicTest() : testCase()
{
    var tester := new VariadicTester();
    Assert.ifTrue(tester.passAndSum(1,2,3)==6);
}
