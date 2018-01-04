import extensions'dynamic.

singleton escript
{
    eval(literal script)
        = TapeAssembly new(scriptEngine 
            load path:"~\scripts\assembly.es";
            load path:"~\scripts\escript.es";
            eval:script); eval.
            
    eval path:path
        = TapeAssembly new(scriptEngine 
            load path:"~\scripts\assembly.es";
            load path:"~\scripts\escript.es";
            eval path:path); eval.
}