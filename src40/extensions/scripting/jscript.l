import extensions'dynamic;

static jscriptEngine
    = new ScriptEngine()
            .loadPath("~\scripts\assembly.es")
            .loadPath("~\scripts\jscript.es");

public singleton jscript
{
    eval(string script)
        = TapeAssembly.load(jscriptEngine.eval(script)).eval();
            
    evalPath(string path)
        = TapeAssembly.load(jscriptEngine.evalPath(path)).eval();
        
    load(string path)
        = TapeAssembly.load(jscriptEngine.loadPath(path)).eval();
}