const int DEFAULT_PORT = 6881.

// --- progrma ---

program =
[
    console writeLine:"ELENA Chat Server".
    
    var server := ChatServer new port:DEFAULT_PORT.
    
    server start.
    
    console readChar.
].