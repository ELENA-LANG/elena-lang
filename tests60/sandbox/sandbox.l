struct X
{
   real r;

   constructor()
   {
      r := 0
   }

   inc()
   {
      r += 1.0
   }

   printMe()
   {
      Console.writeLine(r)
   }
}

public program()
{
   X x := new X();

   x.inc();
   x.printMe();
}
