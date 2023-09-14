import extensions;
 
A
{
   x : prop;
}

public program()
{
   A a := new A();

   a.x := 2;

   var o := a.x;
}