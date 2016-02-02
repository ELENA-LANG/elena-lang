#import system.

#symbol(const,type:int)DEFAULT_PORT = 6881.

// --- progrma ---

#symbol program =
[
    console writeLine:"ELENA Chat Server".
    
    #var server := ChatServer new &port:DEFAULT_PORT.
    
    //#var client := ChatClient new &ip:"127.0.0.1" &port:DEFAULT_PORT.
    
    server start.
    
    console readChar.
].