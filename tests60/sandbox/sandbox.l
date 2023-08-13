import extensions;

struct XY
{
   constructor()
   {
      x := 2;
//      y := 3;
   }

   int x : prop;
//   int y : prop;
}

public program()
{
   XY xy := new XY();

   Assert.ifTrue(xy.x == 2);
//   Assert.ifTrue(xy.y == 3);
}