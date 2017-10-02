
scriptInterpreter = (:script)
(
    scriptEngine 
        load path:"~\scripts\assembly.es";
        load path:"~\scripts\escript.es";
        eval(script);
        eval(Library new); 
        first
).
