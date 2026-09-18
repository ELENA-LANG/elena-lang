public singleton XScript
{
   importBasicTypes()
   {
      Exports["Console"] := Console;
   }
   
   import(string name, object object) 
   {
      Exports[name] := object;
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
