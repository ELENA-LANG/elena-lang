import extensions;

struct XY
{
   int X : prop(_x);

   int Y : prop(_y);

   printMe() { Console.printLine("{X:",_x,",Y:",_y) }
}
 
public program()
{
   XY r := new XY();

   r?.printMe()
}