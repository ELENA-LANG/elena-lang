import extensions;

public program()
{
   console.write("vardispatcherTest:");

   var r1 := VariadicFunctionDispatchTester(1,2,3);
   Assert.ifTrue(r1 == 6);
   console.write(".");
   var r2 := VariadicDispatchTester.product(1,2,3,4);
   Assert.ifTrue(r2 == 24);
   console.write(".");

   var o := VariadicFunctionDispatchTester;
   var o2 := VariadicDispatchTester;
   var r3 := o(1,2,3);
   console.write(".");
   Assert.ifTrue(r3 == 6);
   var r4 := o2.product(1,2,3,4);
   Assert.ifTrue(r4 == 24);

   console.writeLine(".");
}