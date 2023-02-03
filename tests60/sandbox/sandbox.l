import extensions;

A
{
   int x : prop;
}

public program()
{
   var a := new A();
   a.x := 2;

   console.printLine("a.x=", a.x);
}
