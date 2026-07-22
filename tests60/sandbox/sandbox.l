import extensions;

class A
{
   extension printMe<T>(T arg)
   {
      Console.printLine(arg, " was called from ", self)
   }

   constructor() {}
}

public Program()
{
   auto a := new A();
   
   a.printMe(2);
   Assert.ifFailed({ 2.printMe(2) });
}