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

/*
A;

B
{
   A cast() = new A();
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

   var r := dynamic_typecast(o1, o2);
   
}
*/

    const int CTRL_C_EVENT = 0;
    const int CTRL_BREAK_EVENT = 1;
    
    extern int myFunction(uint fdwCtrlType)
    {
       fdwCtrlType =>
          CTRL_C_EVENT : { Console.writeLine("Ctrl-C event"); ^ -1 }
          CTRL_BREAK_EVENT : { Console.writeLine("Ctrl-Break event"); ^ 0 };
          
       ^ 0;      
    }

    public Program()
    {
       extern KERNEL32.SetConsoleCtrlHandler(&myFunction, -1);
       
       Console.writeLine("The Control Handler is installed.");
       Console.writeLine("-- Now try pressing Ctrl+C or Ctrl+Break");
       
       while(true) {}
    }


