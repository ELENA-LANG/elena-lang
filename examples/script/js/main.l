import extensions.
import extensions'dynamic.
import extensions'scripting.

public program =
[
    if (forwards'program'arguments length == 1)
        [  console << "Please provide the path to the file to interpret". AbortException new; raise ].

    try(control do:
    [
        script loadAndEval("grammar_js.es", forwards'program'arguments[1]).
    ])
    {
        on(Exception e)
        [
            console writeLine:e.
        ]
    }.    
].

public stat_program =
[
   console writeLine:"Javascript code executed"
].