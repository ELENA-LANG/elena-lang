import extensions.
import extensions'dynamic.
import extensions'scripting.

program =
[
    if ('program'arguments length == 1)
        [  console << "Please provide the path to the file to interpret". AbortException new; raise ].

    try(control do:
    [
        assemblyScript eval path:"grammar_js.es" path('program'arguments[1]).
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