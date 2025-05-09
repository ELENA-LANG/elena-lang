// Generate a string with N opening brackets ("[") and N closing brackets ("]"), in some arbitrary order.
// Determine whether the generated string is balanced; that is, whether it consists entirely of pairs of opening/closing brackets (in that order),
// none of which mis-nest. 

import system'routines;
import extensions;
import extensions'text;

// --- RandomBrackets ---

randomBrackets(len)
{
   if (0 == len)
   { 
      ^ EmptyString 
   }
   else
   {
      var brackets := 
         Array.allocate(len).populate::(i => $91) 
         + 
         Array.allocate(len).populate::(i => $93);

      brackets := brackets.randomize(len * 2);

      ^ brackets.summarize(new StringWriter()).toString()
   }
}

extension op
{
   get isBalanced()
   {
      var counter := new Integer(0);
    
      self.seekEach::(ch => counter.append((ch==$91).iif(1,-1)) < 0);
    
      ^ (0 == counter)
   }
}

// --- Program ---

public program()
{
   for(int len := 0; len < 9; len += 1)
   {
      var str := randomBrackets(len);

      Console.printLine("""",str,"""",str.isBalanced ? " is balanced" : " is not balanced")
   };

   Console.readChar()
}
