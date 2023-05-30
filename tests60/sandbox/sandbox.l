import extensions;

public program()
{
   uint k := 2H;
   uint l := 0AH;

   var r := k < l;

   Assert.ifFalse(k > l);  // 2 > 10
}