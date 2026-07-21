import extensions;

interface I;

sealed A : I
{
   indexed internal callMethodOutsideInterface()
   {
      Console.writeLine("Invoking a private method");
   }
   
   extension callMe()
   {
      self.callMethodOutsideInterface()
   }

   constructor() {}
}

public Program()
{
   auto a := new A();
   
   a.callMe();
   
   Assert.ifFailed({ 2.callMe() });
}