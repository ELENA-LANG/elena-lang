import extensions;

public program()
{
   uint k := 2H;
   uint l := 0AH;

   var r := k < l;

   Assert.ifTrue(r);  // 2 > 10
}