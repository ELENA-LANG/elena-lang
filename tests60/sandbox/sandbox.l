//import extensions;
//import system'text'parsing;
//
//public program()
//{
//   auto regex1 := new RegEx("^(ht|f)tp(s?)\:\/\/[0-9a-zA-Z]((\.|-+)?[0-9a-zA-Z])*(:(0-9)*)?(\/?)([a-zA-Z0-9\-\.\?\,\'\/\\\+&%\$#_]*)?$");
//
//   Assert.ifTrue(regex1.match("http://localhost"));
//   Assert.ifTrue(regex1.match("http://www.domain.com"));
//   Assert.ifTrue(regex1.match("http://www.domain.com/"));
//   Assert.ifTrue(regex1.match("http://xn--masekowski-d0b.pl"));
//   Assert.ifTrue(regex1.match("http://localhost:9000"));
//
//   Assert.ifTrue(regex1.match("http://www.domain.com/home/index.html"));
//
//
// //  auto regex1 := new RegEx("^[0-9a-zA-Z](\/?)$");
// //  Assert.ifTrue(regex1.match("a"));
// //  Assert.ifTrue(regex1.match("a/"));
//}

//import extensions;
//import net'http;
//
//public program()
//{
//   Uri uri := new Uri("http://127.0.0.1");
//
//   Console.printLine("Scheme:",uri.Scheme," Host:",uri.Host," Port:",uri.Port," Path:",uri.Path," Query:",uri.Query);
//}

A
{
   string s;

   string get()
      = s ?? String.MinValue;

   constructor() {}

   constructor(string s) { this s := s }
}

public program()
{
   A a := new A();
   var r := a.get();
//   console.writeLine(a.get());
//   A a2 := new A("hi");
//   console.writeLine(a2.get());
}