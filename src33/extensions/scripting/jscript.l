import extensions'dynamic.

static $jscript
    = ScriptEngine
            new; 
            load path:"~\scripts\assembly.es";
            load path:"~\scripts\jscript.es".

singleton jscript
{
    eval(literal script)
        = TapeAssembly new($jscript eval:script); eval.
            
    eval path:path
        = TapeAssembly new($jscript eval path:path); eval.
}