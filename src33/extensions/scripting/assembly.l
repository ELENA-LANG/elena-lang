import extensions'dynamic.

static $assemblyScript
    = ScriptEngine
            new; 
            load path:"~\scripts\assembly.es".

singleton assemblyScript
{
    eval(literal script)
        = TapeAssembly new($assemblyScript eval:script); eval.
            
    eval path:path
        = TapeAssembly new($assemblyScript eval path:path); eval.
            
    eval path:grammar path:script
        = TapeAssembly new(
            $assemblyScript load path:grammar;
                eval path:script); eval.
}