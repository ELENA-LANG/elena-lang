import extensions;

struct XY
{
   int x;
   int y;
}

public program()
{
   XY xy := default;

   Console.printLine("size of XY is ", :sizeof xy);
}