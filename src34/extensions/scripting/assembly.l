import extensions'dynamic.

static assemblyScript
    = ScriptEngine
            new; 
            loadPath("~\scripts\assembly.es").

public singleton script
{
    eval(literal script)
        = TapeAssembly new(assemblyScript eval:script); eval.
            
    evalPath(literal path)
        = TapeAssembly new(assemblyScript evalPath:path); eval.

    loadAndEval(literal path, literal script)
        = TapeAssembly new(assemblyScript loadPath:path; evalPath:script); eval.
}