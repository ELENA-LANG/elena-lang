public singleton XScript
{
   importBasicTypes()
   {
      Exports["Console"] := Console;
   }
   
   interpretPath(string path)
   {
      var script := XScriptEngine.buildScriptFile(path).clearImports().compile();
      
      var program := script.Program;
      
      ^ program();
   }
}

// --- Exports ---

public static Exports = system'collections'Dictionary.new();
