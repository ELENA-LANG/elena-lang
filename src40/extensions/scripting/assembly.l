import extensions'dynamic;

static assemblyScript
    = new ScriptEngine()
            .loadPath("~\scripts\assembly.es");

public singleton script
{
    eval(string script)
        = TapeAssembly.load(assemblyScript.eval(script)).eval();
            
    evalPath(string path)
        = TapeAssembly.load(assemblyScript.evalPath(path)).eval();

    loadAndEval(string path, string script)
        = TapeAssembly.load(assemblyScript.loadPath(path).evalPath(script)).eval();
}