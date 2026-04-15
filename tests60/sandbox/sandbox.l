import extensions;
import system'dynamic;

public interface IFunction
{
   abstract real calculate(real arg);
}

public class LinearFunction : IFunction
{
   real a;
   real b;

   constructor(real a, real b)
   {
      this a := a;
      this b := b;
   }

   real calculate(real x)
      = x*a + b;
}
/*
public class Mockup
{
   field object;

   constructor(object)
   {
      this object := object;
   }

   generic cast()
   {
      var type := __received.__getFirstSignatureMember();

      var proxy := ::{ dispatch() => object; };
      proxy.__injectInferface(type);
   
      ^ __received(proxy);
   }
}

public singleton Actor
{
   invoke(IFunction function)
   {
      Console.printLine("Calculating f(1)", function.calculate(1));
   }
}
*/
//public Program()
//{
   //IFunction f := new LinearFunction(2, 3);
//   IFunction f := new LinearFunction(2.3, 3.3);
//
//   auto mssg := new StrongMessage("calculate<system'RealNumber>[2]");
//
//   var found := f.__getClass().respondTo(mssg);
//
//   Console.printLine(f, found ? " responds to " : " does not respond to ", mssg);   
//
//   if (found)
//      Console.printLine(mssg, " returns an instance of ", f.__getMethodOutput(mssg));
//
// var mockup := new Mockup(::{ real calculate(real x) = x * x; });

//   Actor.invoke(new LinearFunction(2, 3));
//   Actor.invoke(mockup);
//}

A
{
   string _origin;
   
   constructor()
   {
      _origin := "created directly"
   }
   
   constructor(B b)
   {
      _origin := "created from B"
   }
   
   string toPrintable()
      = $"A - {_origin}";
}

B
{
   A cast() = new A(self);
}

dynamic_typecast(type_instance,object)
{
   var type := type_instance.__getClass();

   ^ type.__typecast(object);
}

public Program()
{
   var o1 := new A();
   var o2 := new B();

   Console.printLine("An instance of ", o1.__getClass(), " ", dynamic_typecast(o1, o2))   
}
