import extensions;
import extensions'dynamic;

struct XY
{
   int x : prop;
   int y : prop;

   constructor(int x, int y)
   {
      this x := x;
      this y := y
   }

   static XY Default
      = new XY(0, 0);
}

public program()
{
   auto xy := XY.Default;

   console.writeLine(xy.toJson());
}