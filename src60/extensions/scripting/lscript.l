public singleton LScript
{
   eval(string script)
      = LScriptEngine.buildScript(script).compile();
            
   evalPath(string path)
      = LScriptEngine.buildScriptFile(path).compile();
        
   load(string path)
      = LScriptEngine.loadPath(path);
        
   interpret(string script)
   {
      var ns := self.eval(script);
    
      var s := ns.Program;
      ^ s();
   }

   interpretLine(string script)
   {
      string full_script := "Program=" + script + ";";

      var ns := self.eval(full_script);
    
      var s := ns.Program;
      ^ s();
   }

   interpretPath(string path)
   {
      var ns := self.evalPath(path);
    
      var s := ns.Program;
      ^ s()
   }
}

public lscript = LScript; // !! obsolte

// --- Globlas ---

public static Globals = system'collections'Dictionary.new();
