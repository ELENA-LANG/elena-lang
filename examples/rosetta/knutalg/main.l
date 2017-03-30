#import system'dynamic.
#import extensions.
#import system'routines.
#import system'collections.

extension algorithmOp
{
    s_of_n
    [
        var counter := Integer new.
        var n := self.
        
        ^ ArrayList new mix &into:
            {
                eval : i
                [
                    counter += 1.
                    
                    if (self length < n)
                        [ self += i. ];
                        [ 
                            ((randomGenerator eval:counter < n) bool)
                                ? [ self@(randomGenerator eval:n) := i. ].
                        ].
                        
                    ^ self array.                        
                ]
            }.
    ]
}

program =
[
    var bin := Array new:10 set &every:(&index:n) [ Integer new ].
    0 till:10000 &doEach: trial
    [
        var s_of_n := 3 s_of_n.
        
        0 till:10 &doEach:n
        [
            var sample := s_of_n eval:n.
            
            if (n == 9)
                [ sample run &each: i [ bin@i += 1. ]. ].
        ].
    ].    
    
    console writeLine:bin readChar.
].