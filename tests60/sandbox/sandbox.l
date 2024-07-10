import extensions;

textblock A
{
   foo() { console.writeLine("foo") }
}

class B : using(A)
{
   bar() { console.writeLine("bar") }
}

public program()
{
   var b := new B();

   b.foo();
   b.bar();
}