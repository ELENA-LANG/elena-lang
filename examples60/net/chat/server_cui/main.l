import extensions;
import chat_server;

public program()
{
   console.printLine("ELENA Chat Server v.", VERSION);

   auto server := ChatServer.new(DEFAULT_PORT);

   server.OnConnect := { console.writeLine("new client"); };
   server.OnLeave := (string name){ console.printLine(name, " just left"); };
   server.OnError := (int code){ console.printLine("exception #",code) };
   server.OnIntro := (string name){ console.printLine(name, " joined"); };
   server.OnMessage := (string name, string message){ console.printLine(name, ": ", message); };

   server.start();

   console.printLine("To quit, please press any key");
   console.readChar();
}