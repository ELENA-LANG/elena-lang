#import system.
#import extensions'scripting.

#symbol program =
[
    #var factory := scriptEngine 
        load &path:"~\scripts\assembly.es" 
        load &path:"~\scripts\eson.es" 
        eval &path:"test.es".
        
    #var obj := factory eval:(Library new) first eval.
    
    #var n := obj state0.
].