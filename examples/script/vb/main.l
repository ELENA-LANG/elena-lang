#import system.
#import system'dynamic.

#symbol program =
[
    #var a := console.
    #var b := "Hello World".
    #var m := %"writeLine[1]".

    #var aTape := Tape(b,a,m).
            
    aTape eval.
].