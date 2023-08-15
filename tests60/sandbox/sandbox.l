import extensions;

extension op : IntNumber
{
   int X()
      = 2;
}

public program()
{
   int n := 2.X();
}