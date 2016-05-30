#import system.

#symbol(const,int)DEFAULT_PORT = 6881.

// --- progrma ---

#symbol program =
[
    console writeLine:"ELENA Chat Server".
    
    #var server := ChatServer new &port:DEFAULT_PORT.
    
    server start.
    
    console readChar.
].