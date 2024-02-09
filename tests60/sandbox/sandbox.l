import extensions;

sealed class VariadicTester
{
   bool test1Called := false;
   bool test2Called := false;
    
   test1(params int[] others)
   {
      test1Called := true
   }
    
   test2(string s, params int[] others)
   {
      test2Called := true
   }    

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
    
   validate()
   {
      Assert.ifTrue(test1Called).ifTrue(test2Called)
   }
}

public program()
{
    console.write("variadicTest:");
    
    // run-time
    var tester := new VariadicTester();
    tester.test1(1,3,3);
    tester.test2("hello",3,3);
    
    tester.validate();
    console.write(".");

    Assert.ifTrue(tester.sum(1,2,3)==6);
    Assert.ifTrue(tester.passAndSum(1,2,3)==6);
    console.write(".");
    
    // compile-time
    auto tester2 := new VariadicTester();

    tester2.test1(1,3,3);
    tester2.test2("hello",3,3);    
    tester2.validate();
    console.write(".");

    Assert.ifTrue(tester.sum(1,2,3)==6);
    Assert.ifTrue(tester.passAndSum(1,2,3)==6);
    
    console.writeLine(".")
}