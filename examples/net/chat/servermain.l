
// --- program ---

public program()
{
    console.writeLine:"ELENA Chat Server";
    
    auto server := ChatServer.assign(DEFAULT_PORT);

    server.start();

    console.readChar()
}