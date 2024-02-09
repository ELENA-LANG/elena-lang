import extensions;

public program()
{
   string s := "23";

      string intPartStr := nil;
      string decimalPartStr := nil;

      int index := s.indexOf(0, $46);
      if (index != -1) {
         intPartStr := s.Substring(0, index);
         decimalPartStr := s.Substring(index + 1);
      }
      else {
         intPartStr := s;
      };

      console.write(".");

      console.writeLine(intPartStr);

      console.write(".");

      int len := s.Length;
      int val := 0;
      int start := 0;
      bool inverted := false;
      if (s[0]==$45) {
         start := 1;
      };

      console.write(".");

      for (int i := start; i < len; i := i + 1) {
         byte c := s[i];
         int ch := c;
         ch := ch - 48;
         if (ch >= 10) {
            ch := ch - 7;
         };
         val := val * /*radix*/10;
         val := val + ch;
      };



//      int intPart := intConvertor.convert(intPartStr, 10);



//    var r := "23".toReal();

/*    Assert.ifTrue("23".toReal() == 23.0);
    console.write(".");
    Assert.ifTrue("123.456789".toReal() == 123.456789);
    console.write(".");
    Assert.ifTrue("-123.456789".toReal() == -123.456789);*/
    console.write(".");
}