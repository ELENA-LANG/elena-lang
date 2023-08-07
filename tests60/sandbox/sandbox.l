import extensions;

public program()
{
   string s := "23";
//      string intPartStr := nil;
//      string decimalPartStr := nil;

   int index := s.indexOf(0, $46);
   console.writeLine(index);

   console.writeLine(index != -1);      

      if (index != -1) {
//         intPartStr := s.Substring(0, index);
//         decimalPartStr := s.Substring(index + 1);

         console.writeLine(" not -1");
      }
      else {
//         intPartStr := s;

         console.writeLine("-1");
      };

//      console.writeLine(intPartStr);
//
//      bool inverted := false;
//      int intPart := intConvertor.convert(intPartStr, 10);
//      if (intPart < 0) {
//         intPart := -intPart;
//         inverted := true;
//      };
//
//      console.writeLine(intPart);
//
//      real r := intPart;
//      if (decimalPartStr != nil) {
//         real frac := intConvertor.convert(decimalPartStr, 10);
//         for (int i := 0, i < decimalPartStr.Length, i += 1) {
//            frac := frac * 0.1;
//         };
//
//         r += frac;
//      };
//
//      if (inverted)
//         r := r * -1.0;
//
//  console.writeLine(r);


//   int radix := 10;

//      int len := s.Length;

//   console.writeLine(len);

//      int val := 0;
//      int start := 0;
//      bool inverted := false;
//      if (s[0]==$45) {
//         start := 1;
//         inverted := true;
//      };

//      for (int i := start, i < len, i := i + 1) {
//         byte c := s[i];
//         int ch := c;
//         ch := ch - 48;
//         if (ch >= radix) {
//            ch := ch - 7;
//         };
//         val := val * radix;
//         val := val + ch;
//      };

//   console.writeLine(val);

//      if (inverted) {
//         val := -val;
//      };



//   var r := "23".toReal();
//   var n := "23".toInt();

//    Assert.ifTrue("23".toReal() == 23.0);
//    console.write(".");
//    Assert.ifTrue("123.456789".toReal() == 123.456789);
//    console.write(".");
//    Assert.ifTrue("-123.456789".toReal() == -123.456789);
//    console.write(".");
}