import system'collections.
//import system'routines.
import system'net.
import system'threading.
//import system'text.

public sealed class TcpRemoteClient
{
    T<Socket> theSocket.
    
    object prop Parameter :: theParameter.
    
    constructor new(Socket socket)
    [
        theSocket := socket.
    ]
    
    stacksafe read(ByteArray buffer, int maxLength, ref<int> recv)
    [
        theSocket read(buffer, maxLength, 0, recv).
    ]
    
//    stacksafe write bytearray:buffer int:length
//    [
//        int sent := 0.
//        theSocket write bytearray:buffer int:length int:0 vint:sent.
//    ]
    
    close
        => theSocket.
}

//type tcp_server_listener = TcpServerListener.
//limited class TcpServerListener
//{
//    onConnect:client []
//    
//    onLeave:client []
//    
//    onError:e for:client []
//    
//    stacksafe onReceived object:client bytearray:buffer int:length []    
//    
//    tcp_server_listener = $self.
//}

public sealed class TcpServer
{
    T<Socket>             theListenSocket.
    List<TcpRemoteClient> theClients.
    T<Thread>             theListenThread.
    T<ByteArray>          theBuffer.
    int                   theMaxLength.

    Func<TcpRemoteClient,object>               event onConnect  :: theConnectFunc.
    Func<TcpRemoteClient,object>               event onLeave    :: theLeaveFunc.
    Func<TcpRemoteClient,ByteArray,int,object> event onReceived :: theReadFunc.
    Func<TcpRemoteClient,Exception,object>     event onError    :: theErrorFunc.
    
    constructor new(int port)
        <= new(port, 1024).
    
    constructor new(int port, int bufferSize)
    [
        theMaxLength := bufferSize.
        theBuffer := ByteArray new(theMaxLength).

        theClients := List<TcpRemoteClient>().
        
        //resolve server address and port
        T<AddrInfo> addrinfo := AddrInfo new.
        
        // set address info
        addrinfo Family := AF_INET.
        addrinfo SocketType := SOCK_STREAM.
        addrinfo Protocol := IPPROTO_TCP.
        addrinfo Flags  := AI_PASSIVE.
        
        // Create a SOCKET for connecting to server
        T<AddrInfoReader> reader := AddrInfoReader new(port literal, addrinfo).
        
        reader read(addrinfo).        
        int       ai_family := addrinfo Family.
        int       ai_socktype := addrinfo SocketType.
        int       ai_protocol := addrinfo Protocol.
        int       ai_addrlen := addrinfo AddressLen.
        dirty_ptr ai_addrptr := addrinfo Address.

        theListenSocket := Socket new(ai_family, ai_socktype, ai_protocol).
        
        theListenSocket bind(ai_addrptr, ai_addrlen).
        
        reader close.
        
        theListenSocket listen(SOMAXCONN).
    ]

//    run eachConnection:func1
//    [
//        theClients forEach:func1.
//    ]

    set Blocking(bool val)
    [
        int mode := 0.
        
        ifnot (val)
            [
                mode := 1.
            ].

        theListenSocket iocommand(FIONBIO, &mode).
    ]    
    
//    set tcp_server_listener:listener
//    [
//        theListener := listener.
//    ]
    
    private T<TcpRemoteClient> newClient
    [
        auto socket := theListenSocket accept.
        T<TcpRemoteClient> client := nil.
        if (nil != socket)
        [
            socket option(IPPROTO_TCP, TCP_NODELAY, true).
            
            client := TcpRemoteClient new(socket).
            
            theClients append:client
        ].
        
        ^ client
    ]
    
    private stacksafe closeClient(TcpRemoteClient client, int index)
    [
        theClients remove(index).
        
        client close.
                
        if (nil != theLeaveFunc)
            [ theLeaveFunc(client) ].            
    ]
    
    
    private stacksafe handleError(SocketException e) for(TcpRemoteClient client) at(int n)
    [
        if (nil != theErrorFunc)
            [ theErrorFunc(client, e) ].
        
        int err := e error_code.
        if (err == WSAECONNRESET)
            [ self closeClient(client, n) ].
    ]
    
    private readClients
    [
        int i := 0.
        int counter := theClients length.
        
        while (i < counter)
        [
            T<TcpRemoteClient> client := theClients[i].

            int rcvd := 0.
            try(client read(theBuffer, theMaxLength, &rcvd))
            {
                on(SocketException e)
                [
                    self handleError:e for:client at:i.
                ]                
            }.
            
            if (rcvd == 0)
                [ self closeClient(client, i) ];
                [ 
                    if (rcvd > 0)
                    [
                        if (nil != theReadFunc)
                            [ theReadFunc(client,theBuffer,rcvd) ]
                    ]
                ].
            
            i := i + 1
        ].
    ]
    
    private T<bool> proceed
    [
        auto client := self newClient.
        if (nil != client)
        [
            if(nil != theConnectFunc)
                [ theConnectFunc(client) ]
        ].

        self readClients.
        
        ^ true.
    ]
    
    start
    [
        theListenThread := Thread new:
        [
            while (self proceed)[].
        ].
        
        theListenThread start.
    ]
    
//    stacksafe sendAll bytearray:buffer int:length
//    [
//        int i := 0.
//        int counter := theClients length.
//        
//        while (i < counter)
//        [
//            tcp_remoteclient client := theClients getAt int:i; tcp_remoteclient.
//            
//            client write bytearray:buffer int:length.
//            
//            i := i + 1.
//        ].
//    ]
//    
//    stacksafe sendAll literal:s
//    [
//        byte<256> buffer.
//        int       len := s length.
//        int       buf_len := 256.
//        int       index := 0.
//        while (len > 0)
//        [
//            if (len < 256)
//                [ buf_len := len. ];
//                [ buf_len := 256. ].
//            
//            UTF8Encoding convert int:index int:buf_len literal:s bytearray:buffer.
//            
//            $self sendAll bytearray:buffer int:buf_len.
//            
//            len := len - buf_len.
//            index := index + buf_len.
//        ].
//    ]
//    
//    stacksafe sendAll int:n
//    [
//        byte<4> buffer.
//        byteArrayHelper write bytearray:buffer int:0 int:n.
//        
//        $self sendAll bytearray:buffer int:4.
//    ]
//    
//    stacksafe send int:index bytearray:buffer int:length    
//    [
//        theClients getAt int:index; tcp_remoteclient; write bytearray:buffer int:length.
//    ]
//    
//    close
//    [
//        theListenSocket shutdown int:SD_BOTH.
//        
//        theListenSocket close.
//    ]
}