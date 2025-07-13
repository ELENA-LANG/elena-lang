import system'collections;
import system'net;
////import system'text;
////import system'io;
import system'threading;

import net;

const int BUFFER_SIZE = 128;

public sealed class EventBasedTcpServer
{
   TcpListener       _listener;
   bool              _started;
   Thread            _listenThread;  

   List<TcpClient>   _clients;

   Action<TcpClient> OnConnect               : event;
////   Func<TcpRemoteClient,object> OnLeave                 : event;
   Action<TcpClient,byte[],int> OnReceived   : event;
////   Func<TcpRemoteClient,SocketException,object> OnError : event;
    
   constructor new(int port)
   {
      _started := false;

      _clients := new List<TcpClient>();

      _listener := TcpListener.new(port)
   }

//   Enumerable<TcpRemoteClient> Clients = new Enumerable<TcpRemoteClient>
//   { 
//      Enumerator<TcpRemoteClient> enumerator() => _clients; 
//   };
//    
//   private closeClient(TcpRemoteClient client, int index)
//   {
//      _clients.remove(index);
//        
//      client.close();
//
//      OnLeave?.(client)
//   }
//    
//   private handleError(SocketException e, TcpRemoteClient client, int n)
//   {
//      OnError?.(client, e);
//        
//      int err := e.ErrorCode;
//      if (err == WSAECONNRESET)
//         { closeClient(client, n) }
//   }

   private newClient()
   {
      if (_listener.Pending) {
         auto client := _listener.acceptTcpClient();
      
         _clients.append(client);

         OnConnect?.(client)                  
      };
   }

   private readClients()
   {
      int counter := _clients.Length;
      for (int i := 0; i < counter; i += 1) {
         TcpClient client := _clients[i];

         NetworkStream stream := client.stream();
         if (stream.isDataAvailable) {
            byte buffer[BUFFER_SIZE];
            int bytesRead := stream.read(buffer, BUFFER_SIZE);

            OnReceived?.(client,buffer,bytesRead)
         }
      }
   }

   private proceed()
   {
      newClient();
      readClients();
   }
    
   start()
   {
      _started := true;

      _listenThread := Thread.assign(
      {
         while (_started){
            proceed()
         }
      });

      _listener.start()
   }

//   broadcast(byte[] buffer, int length)
//   {
////      int counter := _clients.Length;
////        
////      for(int i := 0; i < counter; i += 1)
////      {
////         TcpClient client := _clients[i];
////         auto stream := client.stream();
////            
////         stream.write(buffer, length)
////      }
//   }
    
   close()
   {
      _listener.stop();
      _started := false;
      
      _listenThread.join();
   }
}
