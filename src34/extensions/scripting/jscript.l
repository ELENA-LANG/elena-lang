import extensions'dynamic.

static jscriptEngine
    = ScriptEngine
            new; 
            loadPath:"~\scripts\assembly.es";
            loadPath:"~\scripts\jscript.es".

public singleton jscript
{
    eval(literal script)
        = TapeAssembly new(jscriptEngine eval:script); eval.
            
    evalPath(literal path)
        = TapeAssembly new(jscriptEngine evalPath:path); eval.
        
    load(literal path)
        = TapeAssembly new(jscriptEngine loadPath:path); eval.
}