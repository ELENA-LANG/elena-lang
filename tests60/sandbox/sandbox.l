import extensions;

extension op2 : int
{
   int sumAndReturn(int arg)
      = self + arg;
}

public program()
{
   int n := 1;
   int m := 3;

   int r := n.sumAndReturn(m);

   console.printLine("the result is ", r);
}
