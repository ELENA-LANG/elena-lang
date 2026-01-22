import extensions;
import system'dynamic;

interface IA
{
   abstract bar();    
}

interface IB
{
    abstract foo();    
}

class A : IA
{
   bar()
   {
      Console.writeLine("bar")
   }    
}

class B : IB
{
   foo()
   {
      Console.writeLine("foo")
   }    
}

Decorator
{
   field target;
    
   constructor(target)
   {
      this target := target
   }
    
   generic()
   {
      Console.print("decorating ");
        
      __received(target)
   }
        
   generic cast()
   {
      var type := __received.__getFirstSignatureMember();

      var proxy := ::{ dispatch() => self; };
      proxy.__injectInferface(type);
   
      ^ __received(proxy);
   }
}

singleton C
{
   proceed(IA a)
   {
      Console.print(a,":");
      a.bar()
   }
    
   proceed(IB b)
   {
      Console.print(b,":");
      b.foo()
   }
    
   // default handler
   proceed(o)
   {
      Console.printLine(o,": unknown");
   }    
}

public Program()
{
   var a := new A();
   var b := new B();
   var decA := new Decorator(a) as: IA;
   var decB := new Decorator(b) as: IB;

   C.proceed(a);
   C.proceed(b);
   C.proceed(decA);
   C.proceed(decB);
}
