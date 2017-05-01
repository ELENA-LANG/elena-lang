#import extensions.
#import extensions'scripting.

program =
[
    try(control do:
    [
        var factory := scriptEngine 
            load &path:"~\scripts\assembly.es" 
//            eval &path:"test.es".
            eval &path:"grammar1.es".
        
        factory eval:(Library new).
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
//    test'stat'program eval.
].