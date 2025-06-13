import extensions;
/*
public program()
{
   real r := (Pi_value/3).sin();

   Console.printLine("sin(π/3) = ", r)
}
*/

singleton IteratorEnumerable : Enumerable
{
   Enumerator enumerator()
      = IteratorEnumerable.infinitEnumerator();

   yield Enumerator infinitEnumerator()
   {
      :yield 2;
      :yield 5;
      :yield 7;
   }
}

public program()
{
   auto e := IteratorEnumerable.enumerator();

   Assert.ifTrue(e.next());
   Assert.ifTrue(*e == 2);
   Assert.ifTrue(e.next());
   Assert.ifTrue(*e == 5);
   Assert.ifTrue(e.next());
   Assert.ifTrue(*e == 7);
   Assert.ifFalse(e.next());
   Console.write(".");
}
