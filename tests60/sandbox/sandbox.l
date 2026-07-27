import extensions;
import system'collections;
import system'dynamic;
import system'runtime;

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

public singleton DependencyInjector
{
   static Map<object, object> _typeFactories := ::Map<object, object>.new((type => (type.ClassHashCode) shl:4));
   
   extension register<T>(object factory)
   {
      _typeFactories[class T] := factory;
   }
   
   generic cast()
   {
      var type := __received.__getFirstSignatureMember();

      var obj := _typeFactories[type].new();
   
      ^ __received(obj);
   }
}

class ClassToBeInjected
{
   readonly I _i;
   
   constructor new(I i)
   {
      _i := i;
   }
   
   string WhoAmI()
      => _i;   
}

public Program()
{   
   DependencyInjector::register<I>(A);

   var obj := ClassToBeInjected.new(DependencyInjector);
   Console.writeLine(obj.WhoAmI());
}
