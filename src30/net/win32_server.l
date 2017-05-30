#import system.
#import system'collections.
#import system'routines.
#import system'net.
#import system'threading.
#import system'text.

#subject tcp_remoteclient = TcpRemoteClient.

sealed class TcpRemoteClient
{
    socket theSocket.
    object theParameter.
    
    constructor new &socket:socket
    [
        theSocket := socket.
    ]
    
    stacksafe read &bytearray:buffer &int:maxLength &vint:received
    [
        theSocket read &bytearray:buffer &int:maxLength &int:0 &vint:received.
    ]
    
    stacksafe write &bytearray:buffer &int:length
    [
        int sent := 0.
        theSocket write &bytearray:buffer &int:length &int:0 &vint:sent.
    ]
    
    tcp_remoteclient = $self.
    
    set &Parameter:object
    [
        theParameter := object.
    ]
    
    close
        => theSocket.
    
    Parameter = theParameter.
}

#subject tcp_server_listener = TcpServerListener.
limited class TcpServerListener
{
    onConnect:client []
    
    onLeave:client []
    
    onError:e &for:client []
    
    stacksafe onReceived:client &bytearray:buffer &int:length []    
    
    tcp_server_listener = $self.
}

sealed class TcpServer
{
    socket     theListenSocket.
    array_list theClients.
    thread     theListenThread.
    bytearray  theBuffer.
    int        theMaxLength.
    
    tcp_server_listener theListener.
    
    constructor new &port:port
        <= new &port:port &bufferSize:1024.
    
    constructor new &port:aPort &bufferSize:size
    [
        theMaxLength := size int.
        theBuffer := ByteArray new &int:theMaxLength.
        
        theClients := ArrayList new.
        
        // winsock start up
        WSAStartUp22.
        
        //resolve server address and port
        addrinfo addrinfo := 0.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_INET.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        addrinfo setFlags &int:AI_PASSIVE.
        
        dirty_ptr ptr := 0.
        addrinfo readAddrInfo &literal:(aPort literal) &dirty_ptr:ptr.
        
        // Create a SOCKET for connecting to server
        addr_reader aReader := ptr.
        aReader read &addrinfo:addrinfo.
        
        int       ai_family := 0.
        int       ai_socktype := 0.
        int       ai_protocol := 0.
        int       ai_addrlen := 0.
        dirty_ptr ai_addrptr := 0.
        addrinfo readInfo &vint:ai_family &vint:ai_socktype &vint:ai_protocol.
        
        theListenSocket := Socket new &int:ai_family &int:ai_socktype &int:ai_protocol.
        
        addrinfo readAddrPtr &dirty_ptr:ai_addrptr &vint:ai_addrlen.
        
        theListenSocket bind &dirty_ptr:ai_addrptr &int:ai_addrlen.
        
        aReader close.
        
        theListenSocket listen &int:SOMAXCONN.
    ]

    run &eachConnection:func1
    [
        theClients run &each:func1.
    ]

    set &blocking:aValue
    [
        int aMode := 0.
        
        ifnot (aValue)
            [
                aMode := 1.
            ].

        theListenSocket iocommand &int:FIONBIO &vint:aMode.
    ]    
    
    tcp_server = $self.
    
    set &tcp_server_listener:listener
    [
        theListener := listener.
    ]
    
    newClient
    [
        var socket := theListenSocket accept.
        var client := $nil.
        if ($nil != socket)
        [
            socket option &int:IPPROTO_TCP &int:TCP_NODELAY &bool:true.
            
            client := TcpRemoteClient new &socket:socket.
            
            theClients += client.
        ].
        
        ^ client.
    ]
    
    closeClient : client
        <= close &int:(theClients indexOf:client int).
    
    close &int:index
    [
        var client := theClients getAt &int:index.
        
        theClients remove &int:index.
        
        client close.
                
        if ($nil != theListener)
            [ theListener onLeave:client. ].        
    ]
    
    onError:e &for:client
    [
        if ($nil != theListener)
            [ theListener onError:e &for:client. ].
        
        int err := e error_code.
        if (err == WSAECONNRESET)
            [ $self closeClient:client. ].
    ]
    
    readClients
    [
        int i := 0.
        int counter := theClients length.
        
        while (i < counter)
        [
            tcp_remoteclient client := theClients getAt &int:i tcp_remoteclient.
            int              received := 0.
            try(client read &bytearray:theBuffer &int:theMaxLength &vint:received)
            {
                SocketError:e
                [
                    $self onError:e &for:client.
                ]                
            }.
            
            if (received == 0)
                [ $self closeClient:client. ];
                [ 
                    if (received > 0)
                        [ theListener onReceived:client &bytearray:theBuffer &int:received. ].
                ].
            
            i := i + 1.
        ].
    ]
    
    proceed
    [
        var client := $self newClient.
        if ($nil != client)
        [
            if ($nil != theListener)
                [ theListener onConnect:client. ].
        ].
        
        $self readClients.
        
        ^ true.
    ]
    
    start
    [
        theListenThread := Thread new &func:
        [
            #loop $self proceed.
        ].
        
        theListenThread start.
    ]
    
    stacksafe sendAll &bytearray:buffer &int:length
    [
        int i := 0.
        int counter := theClients length.
        
        while (i < counter)
        [
            tcp_remoteclient client := theClients getAt &int:i tcp_remoteclient.
            
            client write &bytearray:buffer &int:length.
            
            i := i + 1.
        ].
    ]
    
    stacksafe sendAll &literal:s
    [
        byte(256) buffer := 0.
        int       len := s length.
        int       buf_len := 256.
        int       index := 0.
        while (len > 0)
        [
            if (len < 256)
                [ buf_len := len. ];
                [ buf_len := 256. ].
            
            UTF8Encoding convert &int:index &int:buf_len &literal:s &bytearray:buffer.
            
            $self sendAll &bytearray:buffer &int:buf_len.
            
            len := len - buf_len.
            index := index + buf_len.
        ].
    ]
    
    stacksafe sendAll &int:n
    [
        byte(4) buffer := 0.
        byteArrayHelper write &bytearray:buffer &int:0 &int:n.
        
        $self sendAll &bytearray:buffer &int:4.
    ]
    
    stacksafe send &int:index &bytearray:buffer &int:length    
    [
        theClients getAt &int:index tcp_remoteclient write &bytearray:buffer &int:length.
    ]
    
    close
    [
        theListenSocket shutdown &int:SD_BOTH.
        
        theListenSocket close.
    ]
}