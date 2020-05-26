import system'routines'stex;
////import system'text.
////import system'io.
import system'threading;
import system'net;
import extensions;
////import extensions'dynamic.
import net;

sealed class ChatMember : BaseClient
{
    TcpRemoteClient theOwner;
    ChatServer      theServer;
    
    prop string Name;

    constructor new()
        <= new()
    {
    }

    constructor(TcpRemoteClient owner, ChatServer server)
        <= new()
    {
        theOwner := owner;
        
        theServer := server
    }    
    
    ChatServer Server = theServer;
}

sealed class ChatServer
{
    object     theLock;
    
    TcpServer theNetworkServer;
    Thread    theBroadcastThread;
    
    constructor assign(int port)
    {
        theLock := new Object();
        
        theNetworkServer := TcpServer.assign(port);
        
        theNetworkServer.Blocking := false
    }
    
    translate(command)
    {
        string json := command.toJson();
        
        theNetworkServer.sendToAll(json.Length).sendToAll(json)
    }
    
    private bool proceed()
    {
        theNetworkServer.Clients.forEach:(TcpRemoteClient client)
        {
            lock (theLock)
            {
                auto member := cast ChatMember(client.Parameter) ?? nil;
                if(nil != member)
                {
                    var command := member.readCommand();
                    if (nil != command)
                    {
                        (command.Command) =>
                            INTRODUCTION
                            {
                                member.Name := command.Value;
                                
                                console.printLine(command.Value," joined")
                            }
                            SEND
                            {
                                console.printLine(member.Name,":",command.Value).
                                
                                member.Server.translate(new CommandDTO
                                                        { 
                                                            this Command := TRANSLATE; 
                                                            this Value := command.Value; 
                                                            this User := member.Name; 
                                                        })
                            };
                    }
                }
            }
        };
        
        ^ true
    }
    
    start()
    {
        theNetworkServer.OnConnect := (TcpRemoteClient client)
        {
            lock (theLock)
            {
                console.writeLine:"new client joint";
            
                client.Parameter := new ChatMember(client, self)
            }                                
        };
        
        theNetworkServer.OnLeave := (TcpRemoteClient client)
        {
            lock (theLock)
            {
                console.writeLine:"client left"
            }
        };
        
        theNetworkServer.OnError := (TcpRemoteClient client, SocketException e)
        {
            lock (theLock)
            {
                console.printLine("exception #",e.ErrorCode)
            }
        };
        
        theNetworkServer.OnReceived := (TcpRemoteClient client, byte[] data, int length)
        {
            lock (theLock)
            {
                cast ChatMember(client.Parameter).write(data, length)
            }                        
        };
        
        theNetworkServer.start();
        
        theBroadcastThread := Thread.assign(
        {
            while(self.proceed()) {}
        });
       
        theBroadcastThread.start()
    }
}