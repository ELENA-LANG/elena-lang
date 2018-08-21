//import system'collections.
//import system'routines.
import system'net.
//import system'threading.
//import system'text.
//
//sealed class TcpRemoteClient
//{
//    socket theSocket.
//    object theParameter.
//    
//    constructor new socket:socket
//    [
//        theSocket := socket.
//    ]
//    
//    stacksafe read bytearray:buffer int:maxLength vint:received
//    [
//        theSocket read bytearray:buffer int:maxLength int:0 vint:received.
//    ]
//    
//    stacksafe write bytearray:buffer int:length
//    [
//        int sent := 0.
//        theSocket write bytearray:buffer int:length int:0 vint:sent.
//    ]
//    
//    tcp_remoteclient = $self.
//    
//    set Parameter:object
//    [
//        theParameter := object.
//    ]
//    
//    close
//        => theSocket.
//    
//    Parameter = theParameter.
//}
//
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
    T<Socket>   theListenSocket.
//    array_list theClients.
//    thread     theListenThread.
//    bytearray  theBuffer.
//    int        theMaxLength.
//    
//    tcp_server_listener theListener.
    
    constructor new(int port)
        <= new(port, 1024).
    
    constructor new(int port, int bufferSize)
    [
//        theMaxLength := size int.
//        theBuffer := ByteArray new int:theMaxLength.
//        
//        theClients := ArrayList new.
        
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
//    
//    newClient
//    [
//        var socket := theListenSocket accept.
//        var client := $nil.
//        if ($nil != socket)
//        [
//            socket option int:IPPROTO_TCP int:TCP_NODELAY bool:true.
//            
//            client := TcpRemoteClient new socket:socket.
//            
//            theClients append:client.
//        ].
//        
//        ^ client.
//    ]
//    
//    closeClient : client
//        <= close int(theClients indexOfElement:client).
//    
//    close int:index
//    [
//        var client := theClients getAt int:index.
//        
//        theClients remove int:index.
//        
//        client close.
//                
//        if ($nil != theListener)
//            [ theListener onLeave:client. ].        
//    ]
//    
//    onError:e for:client
//    [
//        if ($nil != theListener)
//            [ theListener onError:e for:client. ].
//        
//        int err := e error_code.
//        if (err == WSAECONNRESET)
//            [ $self closeClient:client. ].
//    ]
//    
//    readClients
//    [
//        int i := 0.
//        int counter := theClients length.
//        
//        while (i < counter)
//        [
//            tcp_remoteclient client := theClients getAt int:i; tcp_remoteclient.
//            int              received := 0.
//            try(client read bytearray:theBuffer int:theMaxLength vint:received)
//            {
//                on(SocketException e)
//                [
//                    $self onError:e for:client.
//                ]                
//            }.
//            
//            if (received == 0)
//                [ $self closeClient:client. ];
//                [ 
//                    if (received > 0)
//                        [ theListener onReceived object:client bytearray:theBuffer int:received. ].
//                ].
//            
//            i := i + 1.
//        ].
//    ]
//    
//    proceed
//    [
//        var client := $self newClient.
//        if ($nil != client)
//        [
//            if ($nil != theListener)
//                [ theListener onConnect:client. ].
//        ].
//        
//        $self readClients.
//        
//        ^ true.
//    ]
//    
//    start
//    [
//        theListenThread := Thread new func:
//        [
//            while ($self proceed)[].
//        ].
//        
//        theListenThread start.
//    ]
//    
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