import system'routines;

public singleton TextGen
{
   string parseAsLineSeparated(string list, string script)
   {
      auto strings := list.splitByNewLine();
      
      ^ strings.generateFrom(script)
   }
}