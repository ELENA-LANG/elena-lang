import system'collections;
import system'runtime;

public singleton DependencyInjector
{
   static Map<object, object> _typeFactories := ::Map<object, object>.new((type => (type.ClassHashCode) shl:4));
   
   extension register<T>(object factory)
   {
      _typeFactories[class T] := factory;
   }
   
   extension registerSingleton<T>(object singleton)
   {
      _typeFactories[class T] := ::{ new() = singleton; };
   }
   
   generic cast()
   {
      var type := __received.__getFirstSignatureMember();

      var obj := _typeFactories[type].new();
   
      ^ __received(obj);
   }
}

public method injectable_constructor<name>
{
   constructor name()
      <= new(...(DependencyInjector..));
}
