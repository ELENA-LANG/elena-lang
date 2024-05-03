import extensions;

X
{
   int _x;

   internal static int GetX(X x)
   {
      ^ x.getX()
   }

   constructor(int n)
   {
      _x := n
   }

   private int getX()
      = _x;

}

extension op : X
{
   printValue()
   {
      console.writeLine(X.GetX(self));
   }
}

public program()
{
   var x := new X(3);

   x.printValue()
}
