public singleton XScript
{
   interpretPath(string path)
   {
      var script := XScriptEngine.buildScriptFile(path).compile();
      
      var program := script.Program;
      
      ^ program();
   }
}
