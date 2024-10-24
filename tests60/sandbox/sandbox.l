import extensions;
import system'text'parsing;

public program()
{
   auto regex1 := new RegEx("^a$");
   Assert.ifTrue(regex1.match("a"));
   Assert.ifFalse(regex1.match("aa"));
   Assert.ifFalse(regex1.match("b"));

   auto regex2 := new RegEx("^[ab]$");
   Assert.ifTrue(regex2.match("a"));
   Assert.ifFalse(regex2.match("aa"));
   Assert.ifTrue(regex2.match("b"));

   auto regex3 := new RegEx("^a*$");
   Assert.ifTrue(regex3.match("a"));
   Assert.ifTrue(regex3.match("aa"));
   Assert.ifTrue(regex3.match(""));
   Assert.ifFalse(regex3.match("b"));

   auto regex4 := new RegEx("^a+$");
   Assert.ifTrue(regex4.match("a"));
   Assert.ifTrue(regex4.match("aa"));
   Assert.ifFalse(regex4.match(""));
   Assert.ifFalse(regex4.match("b"));

   auto regex5 := new RegEx("^[a-z]+$");
   Assert.ifTrue(regex5.match("abcd"));
   Assert.ifFalse(regex5.match("ab3cd"));

   auto regex6 := new RegEx("^abb?a$");
   Assert.ifTrue(regex6.match("abba"));
   Assert.ifFalse(regex6.match("aca"));
   Assert.ifTrue(regex6.match("aba"));
}


