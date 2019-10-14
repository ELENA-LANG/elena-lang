import extensions'dynamic;

static ScriptEngine lscriptEngine
    = new ScriptEngine()
            .loadPath("~\scripts\grammar.es")
            .loadPath("~\scripts\lscript.es");

public singleton lscript
{
    eval(string script)
        = lscriptEngine.buildScript(script).compiled();
            
    evalPath(string path)
        = lscriptEngine.buildScriptFile(path).compiled();
        
    load(string path)
        = lscriptEngine.loadPath(path);
}