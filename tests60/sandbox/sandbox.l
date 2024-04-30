import extensions;

interface I
{
   abstract callMe();
}

closed B : interface<I>
{
   callMe()
   {
      console.writeLine("callMe");
   }
}

public program()
{
   var b := new B();

   I i := b;
   i.callMe();
}