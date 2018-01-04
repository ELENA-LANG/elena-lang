import extensions'dynamic.

static $escript
    = scriptEngine 
            load path:"~\scripts\assembly.es";
            load path:"~\scripts\escript.es".

singleton escript
{
    eval(literal script)
        = TapeAssembly new($escript eval:script); eval.
            
    eval path:path
        = TapeAssembly new($escript eval path:path); eval.
}