public singleton tscript
{
    eval(string script)
        = tscriptEngine.buildScript(script).compile();
            
    evalPath(string path)
        = tscriptEngine.buildScriptFile(path).compile();
        
//    load(string path)
//        = tscriptEngine.loadPath(path);
        
    interpret(string script)
    {
        var ns := self.eval(script);
    
        var s := ns.program;
        ^ s()
    }       
     
    interpretPath(string path)
    {
        var ns := self.evalPath(path);
    
        var s := ns.program;
        ^ s()
    }        
}