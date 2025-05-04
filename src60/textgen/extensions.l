import system'text;
import extensions'scripting;

public extension textGenOp
{
   string generateFrom(string script)
   {
      string full_script := ScriptGenerator.parse(script);
      auto output := new StringBuilder();
      
      var function := LScript.eval(full_script).program;
      
      function(self, output);
      
      ^ *output
   }
}
