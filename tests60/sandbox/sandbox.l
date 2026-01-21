A;

B
{
   retoverload A get()
      = default;
}

singleton Tester
{
   A test(B b)
     = b.get();
}

public Program()
{
}