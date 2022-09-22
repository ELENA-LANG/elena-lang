import system;

gcTests()
{
   console.write("gcTests");
   for (int i := 0, i < 100000, i := i + 1)
   {
      new IntNumber();
   };
   console.writeLine(".");
}

public program() 
{
   gcTests()
}
