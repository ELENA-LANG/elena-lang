import extensions;

A
{
  field x;

  test()
  {
     int x := 2; // warn there is a field with the same name

     x := 3;
  }
}

public Program()
{
}