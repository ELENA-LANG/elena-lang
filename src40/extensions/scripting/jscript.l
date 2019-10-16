static ScriptEngine jscriptEngine
    = new ScriptEngine()
            .loadPath("~\scripts\grammar.es")
            .loadPath("~\scripts\jscript.es");

public singleton jscript
{
    eval(string script)
        = jscriptEngine.buildScript(script).compiled();
            
    evalPath(string path)
        = jscriptEngine.buildScriptFile(path).compiled();
        
    load(string path)
        = jscriptEngine.loadPath(path);
}