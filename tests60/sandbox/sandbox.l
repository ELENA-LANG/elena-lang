import extensions;

A;
B;

testUnion(Union<A,B> union)
{
   if:not:nil(var A? a := *union) {
      Console.writeLine("A is passed");
   }
   else if:not:nil(var B? b := union.Value) {
      Console.writeLine("B is passed");
   };
}

public Program()
{
   auto u1 := new Union<A,B>(new A());
   auto u2 := new Union<A,B>(new B());
   
   testUnion(u1);
   testUnion(u2);
}
