import extensions;

//struct Aligned
//{
//   byte  b;         // ; 0
//   short w;         // ; 2
//   byte  b2;        // ; 4
//   int   n;         // ; 8
//   byte  b3;        // ; 12
//   long  l;         // ; 16
//}

//       0x112xxx33334xxx55555555

/*
struct XY
{
   byte b1;
   byte b2;
}

struct XY2
{
   byte b1;
   int  n2; 
   byte b3;
}
*/

singleton TestHelper
{
   int getValue()
   {
      ^ 2
   }
}

public program()
{
   int retVal := TestHelper.getValue();

/*   XY a := default;
   XY b := a;

   XY2 c := default;
   XY2 d := c;*/
}
