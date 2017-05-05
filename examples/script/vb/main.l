#import extensions.
#import extensions'scripting.

program =
[
    try(control do:
    [
        var factory := scriptEngine 
            load &path:"~\scripts\assembly.es" 
//            eval &path:"test.es".
            load &path:"grammar1.es"
            eval:"print(""Hello World"");".
        
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