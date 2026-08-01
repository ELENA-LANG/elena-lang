import extensions;
import system'dynamic;

interface I
{
   abstract string WhoAmI();
}

A : interface<I>
{
   constructor new() {}
   
   string WhoAmI()
      = "I'm A";
}

B : interface<I>
{
   constructor new() {}
   
   string WhoAmI()
      = "I'm B";
}

class ClassToBeInjected
{
   readonly I _i;   
   
   constructor new(I i) : injectable_constructor()
   {
      _i := i;
   }
   
   string WhoAmI()
      => _i;   
}

public Program()
{   
   DependencyInjector::register<I>(A);

   var obj := ClassToBeInjected.new();
   Console.writeLine(obj.WhoAmI());
}
