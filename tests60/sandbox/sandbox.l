extension realOp : real
{
   real xpower(int i)
      = i;
}

public program()
{
   int i := 1;

   real r := -1.0.xpower(i);
}