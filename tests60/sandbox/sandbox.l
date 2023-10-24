import system'routines;
import system'dynamic'expressions;


import extensions;
//
//struct XY
//{
//   int x/* : prop*/;
//   int y/* : prop*/;
//
//   embeddable constructor()
//   {
//      x := 1;
//      y := 2;
//   }
//}
// 
//public program()
//{
//   XY xy := new XY();
//
////   var x := xy.x;
//}

struct XY
{
   int x;
   int y;

   constructor()
   {
      x := 1;
      y := 2;
   }

   printMe()
   {
      console.writeLine(x);  
      console.writeLine(y);  
   }

   embeddable constructor(int x)
   {
      this x := x;
      this y := 0;
   }
}

public program()
{
//   XY xy := default;

   XY xy := 2;
   xy.printMe();
//   XY xy := 2;
}

