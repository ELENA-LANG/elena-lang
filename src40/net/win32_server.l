import system'collections;
////import system'routines.
import system'net;
import system'threading;
//import system'text.

public sealed class TcpRemoteClient
{
    Socket theSocket;
    
    prop object Parameter;
    
    constructor assign(Socket socket)
    {
        theSocket := socket
    }
    
    read(byte[] buffer, int maxLength, ref int recv)
    {
        theSocket.read(buffer, maxLength, 0, ref recv)
    }
    
//    write(ByteArray buffer, int length)
//    [
//        int sent := 0.
//        theSocket write(buffer, length, 0, &sent).
//    ]
    
    close()
        => theSocket;
}

public sealed class TcpServer
{
    Socket                theListenSocket;
    List<TcpRemoteClient> theClients;
    Thread                theListenThread;
    byte[]                theBuffer;
    int                   theMaxLength;

    event Func<TcpRemoteClient,object> OnConnect;
    event Func<TcpRemoteClient,object> OnLeave;
    event Func<TcpRemoteClient,byte[],int,object> OnReceived;
    event Func<TcpRemoteClient,SocketException,object> OnError;
    
    constructor assign(int port)
        <= assign(port, 1024);
    
    constructor assign(int port, int bufferSize)
    {
        theMaxLength := bufferSize;
        theBuffer := new byte[](theMaxLength);

        theClients := new List<TcpRemoteClient>();
        
        //resolve server address and port
        AddrInfo addrinfo := new AddrInfo();
        
        // set address info
        addrinfo.Family := AF_INET;
        addrinfo.SocketType := SOCK_STREAM;
        addrinfo.Protocol := IPPROTO_TCP;
        addrinfo.Flags  := AI_PASSIVE;
        
        // Create a SOCKET for connecting to server
        AddrInfoReader reader := new AddrInfoReader(port.Printable, addrinfo);
        
        reader.read(addrinfo);
        int       ai_family := addrinfo.Family;
        int       ai_socktype := addrinfo.SocketType;
        int       ai_protocol := addrinfo.Protocol;
        int       ai_addrlen := addrinfo.AddressLen;
        dirty_ptr ai_addrptr := addrinfo.Address;

        theListenSocket := new Socket(ai_family, ai_socktype, ai_protocol);
        
        theListenSocket.bind(ai_addrptr, ai_addrlen);
        
        reader.close();
        
        theListenSocket.listen(SOMAXCONN)
    }

//    Enumerable<TcpRemoteClient> Clients = Enumerable<TcpRemoteClient>::{ /*embeddable dispatch => theClients.*/Enumerator<TcpRemoteClient> enumerator => theClients. }.

    set Blocking(bool val)
    {
        int mode := 0;
        
        ifnot (val)
        {
            mode := 1
        };

        theListenSocket.iocommand(FIONBIO, ref mode)
    }    
    
    private TcpRemoteClient newClient()
    {
        auto socket := theListenSocket.accept();
        TcpRemoteClient client := nil;
        if (nil != socket)
        {
            socket.option(IPPROTO_TCP, TCP_NODELAY, true);
            
            client := TcpRemoteClient.assign(socket);
            
            theClients.append(client)
        };
        
        ^ client
    }
    
    private closeClient(TcpRemoteClient client, int index)
    {
        theClients.remove(index);
        
        client.close();
                
        if (nil != OnLeave)
            { OnLeave(client) }         
    }
    
    private handleError(SocketException e, TcpRemoteClient client, int n)
    {
        if (nil != OnError)
            { OnError(client, e) };
        
        int err := e.ErrorCode;
        if (err == WSAECONNRESET)
            { self.closeClient(client, n) }
    }
    
    private readClients()
    {
        int counter := theClients.Length;
        for (int i := 0, i < counter, i += 1) {
            TcpRemoteClient client := theClients[i];

            int rcvd := 0;
            try
            {
                client.read(theBuffer, theMaxLength, ref rcvd)
            }
            catch(SocketException e)
            {
                self.handleError(e, client, i)
            };
            
            if (rcvd == 0)
            { 
                self.closeClient(client, i) 
            }
            else
            { 
                if (rcvd > 0)
                {
                    if (nil != OnReceived)
                        { OnReceived(client,theBuffer,rcvd) }
                }
            }
        }
    }
    
    private bool proceed()
    {
        auto client := self.newClient();
        if (nil != client)
        {
            if(nil != OnConnect)
                { OnConnect(client) }
        };

        self.readClients();
        
        ^ true
    }
    
    start()
    {
        theListenThread := Thread.assign(()
        {
            while (self.proceed()){}
        });
        
        theListenThread.start()
    }
    
//    T<TcpServer> sendAll(ByteArray buffer, int length)
//    [
//        int i := 0.
//        int counter := theClients length.
//        
//        while (i < counter)
//        [
//            T<TcpRemoteClient> client := theClients[i].
//            
//            client write(buffer, length).
//            
//            i := i + 1
//        ]
//    ]
//    
//    T<TcpServer> sendAll(literal s)
//    [
//        T<byte,256> buffer.
//        int         len := s length.
//        int         buf_len := 256.
//        int         index := 0.
//        while (len > 0)
//        [
//            if (len < 256)
//                [ buf_len := len. ];
//                [ buf_len := 256. ].
//            
//            UTF8Encoding convert(index, buf_len, s, T<ByteArray>(buffer)).
//            
//            self sendAll(T<ByteArray>(buffer), buf_len).
//            
//            len := len - buf_len.
//            index := index + buf_len.
//        ].
//    ]
//    
//    T<TcpServer> sendAll(int n)
//    [
//        T<byte,4> buffer.
//        byteArrayHelper write(buffer, 0, n).
//        
//        self sendAll(T<ByteArray>(buffer), 4).
//    ]
//    
////    stacksafe send int:index bytearray:buffer int:length    
////    [
////        theClients getAt int:index; tcp_remoteclient; write bytearray:buffer int:length.
////    ]
//    
//    close
//    [
//        theListenSocket shutdown(SD_BOTH).
//        
//        theListenSocket close.
//    ]
}