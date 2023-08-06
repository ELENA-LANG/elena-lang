import extensions;

public program()
{
   Assert.ifTrue("abc"w == "abc"w);
   console.write(".");

   Assert.ifFalse("ab"w == "abc"w);
   console.write(".");

   Assert.ifTrue("ba"w > "ab"w);
   console.write(".");

   Assert.ifTrue("ab"w < "abc"w);
   console.write(".");

   Assert.ifTrue("abc"w + "de"w == "abcde"w);
   console.write(".");

   Assert.ifTrue("abcd"w.indexOf(0, "bc"w)==1);
   console.write(".");

   Assert.ifTrue("abcd"w.indexOf(2, "bc"w)==-1);
   console.write(".");

   Assert.ifTrue("acd"w.insert(1,"b"w) == "abcd"w);
   console.write(".");

   Assert.ifTrue("abecd"w.delete(2,1) == "abcd"w);
   console.write(".");

   Assert.ifTrue("abcd"w.Substring(1,2) == "bc"w);
   console.write(".");

   Assert.ifTrue("abc"w[1] == "b");
   console.write(".");

   Assert.ifFailed({ "abc"w[-1] });
   console.write(".");  

   Assert.ifFailed({ "abc"w[4] });
   console.write(".");
}