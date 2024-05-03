import chat_shared;
import net;

public sealed class ChatMember : BaseClient
{
   ChatServer      _server;

   constructor new(TcpRemoteClient owner, ChatServer server)
      <= super new()
   {
//        theOwner := owner;
//        
      _server := server
   }    

   string Name : prop;

   ChatServer Server
      = _server;
}