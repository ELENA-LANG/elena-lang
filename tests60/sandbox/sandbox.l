import extensions;   

interface ITest
{
   abstract int Index
   {
      get();
   }
}

class ATest
{
   int value;

   constructor(int value)
   {
      this value := value
   }

   get int Index()
   {
      ^ value;
   }

   ITest cast() = new ITest { embeddable dispatch() => self; };
}

public program()
{    
   var a := new ATest(2);

   ITest i := a;

   int n := i.Index;
   Assert.ifTrue(n == 2);
}
