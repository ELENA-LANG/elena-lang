import extensions;
import chat_server;

public program()
{
   console.printLine("ELENA Chat Server v.", VERSION);

   auto server := ChatServer.new(DEFAULT_PORT);
   server.start();

   console.printLine("To quit, please press any key");
   console.readChar();
}