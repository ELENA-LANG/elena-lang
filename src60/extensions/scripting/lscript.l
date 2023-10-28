public singleton lscript
{
   eval(string script)
      = lscriptEngine.buildScript(script).compile();
            
   evalPath(string path)
      = lscriptEngine.buildScriptFile(path).compile();
        
   load(string path)
      = lscriptEngine.loadPath(path);
        
   interpret(string script)
   {
      var ns := self.eval(script);
    
      var s := ns.program;
      ^ s();
   }

   interpretPath(string path)
   {
      var ns := self.evalPath(path);
    
      var s := ns.program;
      ^ s()
   }
}