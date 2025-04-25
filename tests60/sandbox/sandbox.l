/*
import system'routines;
import extensions;
import extensions'text;

public extension QuibbleOp : Array<string>
{
   string quibble()
   {
      ^ self.zipBy(
         self.Length.coundDown().selectBy::(n => n == self.Length ? EmptyString : (n == 1 ? " and " : ", ") ),
         (word, prefix => prefix + word)
      )
      .summarize(StringWriter.load("{")) + "}"
   }
}

public program()
{
   Console.printLine(new string[] { }.quibble());
   Console.printLine(new string[] { "ABC" }.quibble());
   Console.printLine(new string[] { "ABC", "ADF" }.quibble());
   Console.printLine(new string[] { "ABC", "DEF", "G", "H" }.quibble())
}
*/

import textgen;

public program()
{
   //var text := "Hello World!!".generateFrom("<={self}=>");
   //var text := "Hello World!!".generateFrom("<=Hello!!=>");

   var text := 5.generateFrom("for(var n := 0; n < self; n := n + 1) { <={n}=> }");

   Console.writeLine(text);
}