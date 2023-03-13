import extensions;

public program()
{
   for (int i := 0, i < 20, i += 1)
   {
      var r := randomGenerator.nextInt(20);

      console.writeLine(r);
   }
}
