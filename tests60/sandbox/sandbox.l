public const struct UShortNumber
{
   embeddable __uint _value[2];

   constructor(int n)
            : info("tries to convert the integer number o to unsigned short number")
      = UShortConvertor.convert(n);
}

struct X
{
   int x;

   constructor(int x)
   {
      this x := x
   }
}

public program()
{
   X x := 2;
}
