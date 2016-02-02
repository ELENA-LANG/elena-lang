#import system.

#symbol(const,type:int)DEFAULT_PORT = 6881.

#symbol program =
[
    console writeLine:"ELENA Char Client".
    
    #var client := ChatClient new &ip:"127.0.0.1" &port:DEFAULT_PORT.
    
    client start
        | if &Error: e [ console writeLine:"Cannot esteblish connection!". ].
    
    console readChar.
    
    client stop.
].
