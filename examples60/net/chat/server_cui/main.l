import extensions;
import chat_server;

public program()
{
   Console.printLine("ELENA Chat Server v.", VERSION);

   auto server := ChatServer.new(DEFAULT_PORT);

   server.OnConnect := { Console.writeLine("new client"); };
   server.OnLeave := (string name){ Console.printLine(name, " just left"); };
   server.OnError := (int code){ Console.printLine("exception #",code) };
   server.OnIntro := (string name){ Console.printLine(name, " joined"); };
   server.OnMessage := (string name, string message){ Console.printLine(name, ": ", message); };

   server.start();

   Console.printLine("To quit, please press any key");
   Console.readChar();
}