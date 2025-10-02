interface I
{
   abstract foo();
}

class A : interface<I>
{
   bar() {}

/*   I cast() = new I
   {
      embeddable dispatch() => self;
   };*/
}

public program()
{
}
