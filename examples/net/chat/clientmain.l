#import system.

#symbol(const,int)DEFAULT_PORT = 6881.

#symbol program =
[
    console writeLine:"ELENA Char Client".
    
    #var userName := console write:"Enter nickname:" readLine.
    
    #var client := ChatClient new &ip:"127.0.0.1" &port:DEFAULT_PORT &userName:userName.
    
    client start
        | if &Error: e [ console writeLine:"Cannot esteblish connection!". ].
    
    console readChar.
    
    client stop.
].
