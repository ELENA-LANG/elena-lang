#import system.
#import extensions'scripting.

#symbol program2 =
[
    #var n := script_test'mytest'States state0.
].

#symbol program =
[
    #var factory := scriptEngine 
        load &path:"~\scripts\assembly.es" 
        load &path:"~\scripts\eson.es" 
        eval &path:"test.es".
        
    #var obj := factory eval:(Library new) first eval.
    
    #var n := obj state0.
    #var m := script_test'mytest'States state0.
    
    #var nn := n digit:3.
    #var mm := m digit:3.
].