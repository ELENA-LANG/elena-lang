public symbol staticRules =
    calc'rules'States;

public singleton control
{
   greeting() = "Enter the expression to evaluate (e.g. 2+2) or -q to exit";
    
   Prompt = ">";
    
   proceed(line)
   {
      if ("-q" == line)
      {
         forward program.stop(); ^emptyString
      }
      else
      {
         ^ line.evaluatedWith(forward rules)
      }   
   }    
}
