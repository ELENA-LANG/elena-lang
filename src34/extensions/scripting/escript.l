import extensions'dynamic.

static escriptEngine
    = ScriptEngine
            new; 
            loadPath("~\scripts\assembly.es");
            loadPath("~\scripts\escript.es").

public singleton escript
{
    eval(literal script)
        = TapeAssembly new(escriptEngine eval:script); eval.
            
    load(literal path)
        = TapeAssembly new(escriptEngine loadPath:path); eval.
}