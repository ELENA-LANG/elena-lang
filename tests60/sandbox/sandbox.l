import extensions;

A
{
   const subject[] dto_properties;

   this dto_properties += mssg test;

   printDto()
   {
      console.printLine(dto_properties.asEnumerable());
   }
}

B : A
{
   this dto_properties += mssg test_b;
}

public program()
{
   var a := new A();
   var b := new B();

   console.writeLine("A:");
   a.printDto();
   console.writeLine("B:");
   b.printDto();
}
