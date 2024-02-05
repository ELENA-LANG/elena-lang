import extensions;

A
{
   const subject[] dto_properties;

   this dto_properties += mssg test;

   printDto()
   {
      var o := dto_properties;
         var m := o[0];

      console.writeLine(dto_properties[0]);
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
