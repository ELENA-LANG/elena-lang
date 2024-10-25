//import extensions;
//import system'text'parsing;
//
//public program()
//{
//   auto regex1 := new RegEx("^a$");
//   Assert.ifTrue(regex1.match("a"));
//   Assert.ifFalse(regex1.match("aa"));
//   Assert.ifFalse(regex1.match("b"));
//   Console.write(".");
//
//   auto regex2 := new RegEx("^[ab]$");
//   Assert.ifTrue(regex2.match("a"));
//   Assert.ifFalse(regex2.match("aa"));
//   Assert.ifTrue(regex2.match("b"));
//   Console.write(".");
//
//   auto regex3 := new RegEx("^a*$");
//   Assert.ifTrue(regex3.match("a"));
//   Assert.ifTrue(regex3.match("aa"));
//   Assert.ifTrue(regex3.match(""));
//   Assert.ifFalse(regex3.match("b"));
//
//   auto regex4 := new RegEx("^a+$");
//   Assert.ifTrue(regex4.match("a"));
//   Assert.ifTrue(regex4.match("aa"));
//   Assert.ifFalse(regex4.match(""));
//   Assert.ifFalse(regex4.match("b"));
//   Console.write(".");
//
//   auto regex5 := new RegEx("^[a-z]+$");
//   Assert.ifTrue(regex5.match("abcd"));
//   Assert.ifFalse(regex5.match("ab3cd"));
//   Console.write(".");
//
//   auto regex6 := new RegEx("^abb?a$");
//   Assert.ifTrue(regex6.match("abba"));
//   Assert.ifFalse(regex6.match("aca"));
//   Assert.ifTrue(regex6.match("aba"));
//   Console.write(".");
//
//   auto regex7 := new RegEx("^(ht|f)$");
//   Assert.ifTrue(regex7.match("ht"));
//   Assert.ifTrue(regex7.match("f"));
//   Assert.ifFalse(regex7.match("http"));
//   Console.write(".");
//
//
//
//   //auto regex1 := new RegEx("^(ht|f)tp(s?)\:\/\/[0-9a-zA-Z]([-\.\w]*[0-9a-zA-Z])*$");
//
//   //Assert.ifTrue(regex1.match("http://ab"));
////   Assert.ifTrue(regex1.match("ftp://A-"));
////   Assert.ifTrue(regex1.match("ftp://a."));
////   Assert.ifTrue(regex1.match("ftp://0."));
//
////   auto regex1 := new RegEx("^(ht|f)tp(s?)\:\/\/[0-9a-zA-Z]((\.|-+)?[0-9a-zA-Z])*(:(0-9)*)?(\/?)$");
////
////   Assert.ifTrue(regex1.match("http://localhost"));
////   Assert.ifTrue(regex1.match("http://www.domain.com"));
////   Assert.ifTrue(regex1.match("http://www.domain.com/"));
////   Assert.ifTrue(regex1.match("http://xn--masekowski-d0b.pl"));
////   Assert.ifTrue(regex1.match("http://localhost:9000"));
//
//
// //  auto regex1 := new RegEx("^[0-9a-zA-Z](\/?)$");
// //  Assert.ifTrue(regex1.match("a"));
// //  Assert.ifTrue(regex1.match("a/"));
//}
//
//

class A
{
   A? check(int n)
      = n == 0 ? self : nil;
}

public program()
{
   A a := new A();

   A r := a.check(2);
}

