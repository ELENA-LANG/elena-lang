import extensions;

class Object
{
   dispatch() => nil;
}

public interface Func
{
   abstract function();
}

singleton E
{
   callFromLambda()
   {
      var f := { callMe(); };

      f();
   }

   private callMe(){}
}

public program()
{
   E.callFromLambda()
}