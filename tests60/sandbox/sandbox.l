import extensions;

singleton helper
{
   get(out int n)
   {
      n := 2;
   }
}

public program()
{
   helper.get(out int r);
}
