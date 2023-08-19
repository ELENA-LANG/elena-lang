import extensions;

singleton VariadicFunctionDispatchTester
{
   function(params int[] args)
   {
      int l := args.Length;
      int sum := 0;
      for (int i := 0, i < l, i += 1) {
         sum := sum + args[i]
      };

      ^ sum
   }
}

singleton VariadicDispatchTester
{
   product(params int[] args)
   {
      int l := args.Length;
      int prod := 1;
      for (int i := 0, i < l, i += 1) {
         prod := prod * args[i]
      };

      ^ prod
   }
}

public program()
{
   var o := VariadicFunctionDispatchTester;
   var o2 := VariadicDispatchTester;
   var r3 := o(1,2,3);
   console.write(".");
//   Assert.ifTrue(r3 == 6);
//   var r4 := o2.product(1,2,3,4);
//   Assert.ifTrue(r4 == 24);

//   console.writeLine(".");
}
