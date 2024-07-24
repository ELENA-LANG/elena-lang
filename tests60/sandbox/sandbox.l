import extensions;

B;

A
{
   B b;

   constructor new(B? b)
   {
      this b := b
   }
}

public program()
{
   var b := nil;
//   var t := A;
//   var a1 := t.new(b);
   var a2 := A.new(b);
}