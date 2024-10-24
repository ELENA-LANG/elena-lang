import extensions;
import system'text'parsing;

public program()
{
   //auto regex1 := new RegEx("^(ht|f)tp(s?)\:\/\/[0-9a-zA-Z]([-\.\w]*[0-9a-zA-Z])*$");

   //Assert.ifTrue(regex1.match("http://ab"));
//   Assert.ifTrue(regex1.match("ftp://A-"));
//   Assert.ifTrue(regex1.match("ftp://a."));
//   Assert.ifTrue(regex1.match("ftp://0."));

//   auto regex1 := new RegEx("^(ht|f)tp(s?)\:\/\/[0-9a-zA-Z]((\.|-+)?[0-9a-zA-Z])*(:(0-9)*)?(\/?)$");
//
//   Assert.ifTrue(regex1.match("http://localhost"));
//   Assert.ifTrue(regex1.match("http://www.domain.com"));
//   Assert.ifTrue(regex1.match("http://www.domain.com/"));
//   Assert.ifTrue(regex1.match("http://xn--masekowski-d0b.pl"));
//   Assert.ifTrue(regex1.match("http://localhost:9000"));


   auto regex1 := new RegEx("^[0-9a-zA-Z](\/?)$");
   Assert.ifTrue(regex1.match("a"));
   Assert.ifTrue(regex1.match("a/"));
}


