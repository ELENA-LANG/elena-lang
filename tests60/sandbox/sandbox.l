import extensions;

   string convert(int value, int radix)
   {
      int n := value;
      if (n < 0)
      {
         if (n == 80000000h) {
            ^ convert(-2147483648l, radix);
         };
         
         n := 0 - n;
      };   

      byte buffer[36];
      byte tmp;
      int len := 0;
      int index := 35;
      while (n >= radix) {
         int r := n.mod(radix);
         n := n / radix;

         tmp := 48 + r;
         if (r > 9) {
            tmp :=tmp + 7;
         };

         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := 48 + n;
      if (n > 9) {
         tmp :=tmp + 7;
      };

      buffer[index] := tmp;
      len := len + 1;

      if (value < 0) {
        index := index - 1;
        tmp := 45;
        buffer[index] := tmp;
        len := len + 1;
      };

      ^ system'text'UTF8Encoding.toString(index, len, buffer);
   }



public Program()
{
/*   long n := 20;
   int r := 3;

   long rest := n.mod(r);
   n := n / r;

   Console.writeLine(rest);
   Console.writeLine(n);

   long m := -6;
   m := 0 - m;
   Console.writeLine(m);

*/

   string s := convert(-2147483, 10);
   Console.write(s); 

//   int n := 2;
//   int m := 3;
//   int k := 4;
//   int l := 3;
                    
//   n := /*IntNumber.MinValue*//*-2147483647*/-214748/*364*/;
//   string s := n.toPrintable();
//   Console.writeLine(s);
//   Assert.ifTrue(s == "-2147483648");
//   Console.write("."); 
}