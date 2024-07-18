import extensions;

textblock Shared
{
   foo()
   {
      console.writeLine("foo")
   }
}

class A : using(Shared)
{
   bar()
   {
      console.writeLine("bar")
   }
}

public program()
{
   var a := new A();

   a.foo();
   a.bar();
}
