import extensions;

public program()
{
   uint k := 2H;
   uint l := 0AH;

   var r := l < k;

   Assert.ifTrue(r);  // 2 > 10
}