#import extensions.
#import extensions'scripting.

program =
[
    try(control do:
    [
        var factory := scriptEngine 
            load &path:"~\scripts\assembly.es" 
            eval &path:"grammar1.es".
        
        var obj := factory eval:(Library new) first eval.
        
        obj eval.
    ])
    {
        ! e
        [
            console writeLine:e.
        ]
    }.    
].

stat_program =
[
    test'stat'program eval.
].