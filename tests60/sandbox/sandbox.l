import extensions;

public const struct Color : enum<int>(Red = 1,Green = 2,Blue = 3);

public enumTest()
{
   Color red := Color.Red;
   Color green := Color.Green;

   Assert.ifTrue(red == Color.Red);
   Assert.ifFalse(red == Color.Green);
   Assert.ifEqual(red.toPrintable(), "Red");
   Assert.ifEqual(green.toPrintable(), "Green");

   console.write(".");   
}

public program()
{
   enumTest();

   foreach(Color c; in Color)
   {
      console.writeLine(c);
   }
}
