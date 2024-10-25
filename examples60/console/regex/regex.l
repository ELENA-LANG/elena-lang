import extensions;
import system'text'parsing;

public singleton Control
{
   static RegEx _regEx;

   greeting()
   {
      string pattern := Console.print("Please enter a regular expression to be tested against:").readLine();
      
      _regEx := new RegEx(pattern);
   }


    get string Prompt
        = "Please enter a string to validate:";

   proceed(line)
      = _regEx.match(line) ? "matched" : "not matched";
}