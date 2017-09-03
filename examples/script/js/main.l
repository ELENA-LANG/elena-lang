import extensions.
import extensions'scripting.

program =
[
//    if ('program'arguments length == 1)
//        [  console << "Please provide the path to the file to interpret". AbortException new; raise ].

    var p := "test.js".

    try(control do:
    [
        var factory := scriptEngine 
            load path:"~\scripts\assembly.es";
            load path:"grammar_js.es";
            eval path(/*'program'arguments[1]*/p).
            
        factory eval:(Library new).
    ])
    {
        on(Exception e)
        [
            console writeLine:e.
        ]
    }.    
].

stat_program =
[
   console writeLine:"Javascript code executed"
].