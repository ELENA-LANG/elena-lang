import extensions;

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
public Program()
{
   IFunction f := new LinearFunction(2, 3);

   auto mssg := new StrongMessage("calculate<system'Real>[2]");

   /*var mockup := new Mockup(::{ real calculate(real x) = x * x; });

   Actor.invoke(new LinearFunction(2, 3));
   Actor.invoke(mockup);*/
}
