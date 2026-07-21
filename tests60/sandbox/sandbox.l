import extensions;

class A
{
   public extension op
   {
      printMe()
      {
         Console.writeLine(self.toPrintable())
      }
   }
}

public Program()
{
   auto a := new A();
   
   a.printMe();
   
   Assert.ifFailed({ 2.printMe() });
}