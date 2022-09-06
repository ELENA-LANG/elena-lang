import system;
import extensions;

singleton BoxingProber
{
   check(int n)
   {
      Assert.ifTrue(n == 123);
   }
}

boxingTests()
{
   console.write("boxingTests");

   int n := 123;

   // 1. passing directly
   BoxingProber.check(n);
   console.write(".");

   // 1. weak passing directly
   var o := BoxingProber;
   o.check(n);
   console.write(".");

   console.writeLine(".");
}
