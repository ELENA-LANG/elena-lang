import extensions;
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

//B : interface<I>
//{
//   constructor new() {}
//   
//   string WhoAmI()
//      = "I'm B";
//}
//
//public singleton DependencyInjector
//{
//   static Map<object, object> _typeFactories := Map<object, object>.new((type => type.ClassHashCode ));
//   
//   generic cast()
//   {
//      var type := __received.__getFirstSignatureMember();
//
//      var obj := type.new();
//   
//      ^ __received(obj);
//   }   
//}
//
//class ClassToBeInjected
//{
//   readonly I _i;
//   
//   constructor new(I i)
//   {
//      _i := i;
//   }
//}

public Program()
{
   int id := A.ClassHashCode;
   int id2 := (A.new()).ClassHashCode;
   
//   var obj := ClassToBeInjected.new(TypeInjector);
//   obj.WhoAmI();

   
}
