import extensions;
import system'dynamic;

interface IDecoratorA
{
   abstract giveMe();
}

class DecoratorBaseA : IDecoratorA
{
   field value;

   constructor(value)
   {
      this value := value
   }

   giveMe()
      = value;
}

class DecoratorA
{
   object target;
   object extra;

   constructor(target, extra)
   {
      this target := target;
      this extra := extra;
   }

   giveMe()
      = target.giveMe() + extra;

   generic cast()
   {
      var type := __received.__getFirstSignatureMember();

      var proxy := ::{ dispatch() => self; };
      proxy.__injectInferface(type);
   
      ^ __received(proxy);
   }
}

decoratorTest()
{
   var a := new DecoratorBaseA(2);
   var decA := new DecoratorA(a, 3);

   Assert.ifEqual((decA :as IDecoratorA).giveMe(), 5);
}

public program()
{
   decoratorTest();
}
