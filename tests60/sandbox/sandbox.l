import extensions;

singleton helper
{
   int returnValue()
      = 3;

   inc(ref int n)
   {
      n := n + 1;
   }

   get(out int n)
   {
      n := 2;
   }
}

public program()
{
   helper.inc(ref int v := 0);
   
   helper.get(out int r);

   console.writeLine(v);
   console.writeLine(r);


   int m := helper.returnValue();
}
