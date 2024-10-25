import extensions;
import system'text'parsing;

public singleton Control
{
   static RegEx _regEx;

   greeting()
   {
      string pattern := Console.print("Please enter a regular expression to be tested against ( e.g. ^\w+$ ):").readLine();
      
      _regEx := new RegEx(pattern);

      ^ $"The pattern is {pattern}";
   }


   get string Prompt
      = "Please enter a string to validate or press ENTER to exit:";

   proceed(line)
   {
      if (line == String.MinValue) 
        forward program.stop();

      ^ _regEx.match(line) ? "matched" : "not matched";
   }
}