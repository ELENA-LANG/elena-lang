#import system.
#import extensions.

#symbol program =
[
    #var s := String new:"Hello".
    s += " World".
    
    console writeLine:s readChar.
].