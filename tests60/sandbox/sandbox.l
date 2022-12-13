import extensions;

A
{
   cast generic() = nil;
}

B;

public program()
{
   var a := new A();
   B b := a;
}
