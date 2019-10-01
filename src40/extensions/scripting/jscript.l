import extensions'dynamic;

static lscriptEngine
    = new ScriptEngine()
            .loadPath("~\scripts\grammar.es")
            .loadPath("~\scripts\lscript.es");

public singleton lscript
{
    eval(string script)
        = lscriptEngine.buildScript(script).compled();
            
    evalPath(string path)
        = lscriptEngine.buildScriptFile(path).compled();
        
    load(string path)
        = lscriptEngine.loadPath(path);
}