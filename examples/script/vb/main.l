import extensions.
import extensions'scripting.

program =
[
    try(control do:
    [
        var factory := scriptEngine 
            load path:"~\scripts\assembly.es";
//            eval &path:"test.es".
            load path:"grammar_js.es";
//            eval &path:"test_js1.es".
            eval path:"test_js2.es".
            
//            eval:"print(""Hello World"");".
        
        factory eval:(Library new).
    ])
    {
        generic : e
        [
            console writeLine:e.
        ]
    }.    
].

stat_program =
[
//    test'stat'program eval.
].