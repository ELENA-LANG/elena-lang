import extensions'dynamic.

singleton jscript
{
    eval(literal script)
        = TapeAssembly new(scriptEngine 
            load path:"~\scripts\assembly.es";
            load path:"~\scripts\jscript.es";
            eval:script); eval.
            
    eval path:path
        = TapeAssembly new(scriptEngine 
            load path:"~\scripts\assembly.es";
            load path:"~\scripts\jscript.es";
            eval path:path); eval.
}