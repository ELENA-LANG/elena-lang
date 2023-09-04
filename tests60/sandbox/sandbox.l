struct X
{
   int x;

   constructor()
   {
      x := 0;
   }

   get()
   {
      var r := self;

      ^ r
   }
}

public program()
{
   X x := new X();
   var r := x.get();

   var x2 := new X();
   var r2 := x2.get();

}