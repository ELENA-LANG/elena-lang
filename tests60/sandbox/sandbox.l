import extensions;

public program()
{
   uint m := 80000000H;
   uint n := 10H;
   uint k := m / n;
   Assert.ifTrue(k == 8000000H);


//   var n := uintConvertor.convert("0FFFFFFFF", 16);
//   var s := n.toPrintable();


//   int  n1  := 2;             // a 32bit integer literals
//   int  n2 := -23; 
//   int  x1 := 0Fh;            // a signed 32bit hexadecimal integer        
//   uint x2 := 0FH;            // an unsigned 32bit hexadecimal integer        
//   long l1 := 1234567890123l; // a 64bit integer literals
//   long l2 := -233544364345l;
//   real r1 := 23.2;          // a floating-point literals
//   real r2 := 1.2e+11;
}