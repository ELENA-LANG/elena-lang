import extensions;

singleton Tester
{
   test(int? prm)
   {
      ^ prm.hasValue ? (prm.Value) : 0
   }
}

public program()
{
   var t := Tester;

   console.printLine("Tester.test(2)=",Tester.test(2));
   console.printLine("t.test(3)=",t.test(3));

   console.printLine("Tester.test(nil)=",Tester.test(nil));
   console.printLine("t.test(nil)=",t.test(nil));
}
