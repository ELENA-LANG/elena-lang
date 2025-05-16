import system'text;

public extension textGenOp
{
   string generateFrom(string script)
   {
      string full_script := ScriptGenerator.parse(script);
      auto output := new StringBuilder();

      auto scriptEngine := new ScriptEngine()
            .loadScript("[[ #grammar build ]]")
            .loadPath("~\scripts\grammar60.es")
            .loadPath("~\scripts\lscript60.es")

      var function := scriptEngine.eval(full_script).program;
      
      function(self, output);
      
      ^ *output
   }
}
