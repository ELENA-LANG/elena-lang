import extensions'dynamic.

singleton assemblyScript
{
    eval(literal script)
        = TapeAssembly new(scriptEngine 
            load path:"~\scripts\assembly.es";
            eval:script); eval.
            
    eval path:path
        = TapeAssembly new(scriptEngine 
            load path:"~\scripts\assembly.es";
            eval path:path); eval.
            
    eval path:grammar path:script
        = TapeAssembly new(scriptEngine 
            load path:"~\scripts\assembly.es";
            load path:grammar;
            eval path:script); eval.
}