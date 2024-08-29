import extensions;

A;

singleton Helper
{
   getInstance()
   {
      console.writeLine("calling getInstance");

      ^ new A();
   }
}

public program()
{
   var a := /*new A()*/nil;

   a := a ?? Helper.getInstance()
}
