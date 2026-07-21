//import extensions;

singleton A
{
   class B
   {
      testMe()
      {
         Console.writeLine("A::B testMe")
      }
   }

   testB()
   {
      B b := new B();

      b.testMe();
   }
}

public Program()
{
   A.testB()
}