import extensions;

WSData
{
   x;

   constructor new(byte n)
   {
      x := "byte"
   }

   constructor new(int n)
   {
      x := "int"
   }
}

public program()
{
   var r := WSData.new(2);
   var c := WSData;
   var r2 := c.new(2);
}