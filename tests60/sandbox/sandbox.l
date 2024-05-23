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
      console.writeLine("bar")
   }    
}

class B : IB
{
   foo()
   {
      console.writeLine("foo")
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
      console.print("decorating ");
        
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
      console.print(a,":");
      a.bar()
   }
    
   proceed(IB b)
   {
      console.print(b,":");
      b.foo()
   }
    
   // default handler
   proceed(o)
   {
      console.printLine(o,": unknown");
   }    
}

public program()
{
   var a := new A();
   var b := new B();
   var decA := new Decorator(a) :as IA;
   var decB := new Decorator(b) :as IB;

   C.proceed(a);
   C.proceed(b);
   C.proceed(decA);
   C.proceed(decB);
}
