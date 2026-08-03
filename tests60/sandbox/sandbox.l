import extensions;
import system'drawing;

struct XY
{
   int X : prop(x);
   int Y : prop(y);
   
   constructor(int x, int y)
   {
      this x := x;
      this y := y;
   }
   constructor(int x)
   {
      this x := x;
      this y := 0;
   }
}

singleton Tester
{
   test(XY xy)
   {
      int x := xy.X; 
   }
}

public Program()
{   
   XY xy := new XY(3);
   
   Tester.test(xy)
}
