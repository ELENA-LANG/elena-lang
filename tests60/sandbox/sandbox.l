import extensions;

struct X
{
   int x;

   constructor()
   {
      x := 0
   }
}

singleton Helper
{
   test(X x)
      = x;
}

public Program()
{
   var o := Helper;
   var x := o.test(new X());
}