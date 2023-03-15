import extensions;

class A
{
   field target;

   constructor(o)
   {
      target := o
   }
}

public program()
{
   auto o := new A(2);

   var r := o.add(2);
}
