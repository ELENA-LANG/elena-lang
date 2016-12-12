#import system.
#import system'collections.
#import system'routines.
#import system'net.
#import system'threading.
#import system'text.

#subject tcp_remoteclient = TcpRemoteClient.
#class(sealed)TcpRemoteClient
{
    #field(socket)theSocket.
    #field theParameter.
    
    #constructor new &socket:socket
    [
        theSocket := socket.
    ]
    
    #method(stacksafe) read &bytearray:buffer &int:maxLength &vint:received
    [
        theSocket read &bytearray:buffer &int:maxLength &int:0 &vint:received.
    ]
    
    #method(stacksafe) write &bytearray:buffer &int:length
    [
        #var(int)sent.
        theSocket write &bytearray:buffer &int:length &int:0 &vint:sent.
    ]
    
    #method tcp_remoteclient = $self.
    
    #method set &Parameter:object
    [
        theParameter := object.
    ]
    
    #method close
        => theSocket.
    
    #method Parameter = theParameter.
}

#subject tcp_server_listener = TcpServerListener.
#class(limited) TcpServerListener
{
    #method onConnect:client []
    
    #method onLeave:client []
    
    #method onError:e &for:client []
    
    #method(stacksafe)onReceived:client &bytearray:buffer &int:length []    
    
    #method tcp_server_listener = $self.
}

#class(sealed)TcpServer
{
    #field(socket)theListenSocket.
    #field(array_list)theClients.
    #field(thread)theListenThread.
    #field(bytearray)theBuffer.
    #field(int)theMaxLength.
    
    #field(tcp_server_listener)theListener.
    
    #constructor new &port:port
        <= new &port:port &bufferSize:1024.
    
    #constructor new &port:aPort &bufferSize:size
    [
        theMaxLength := size int.
        theBuffer := ByteArray new &int:theMaxLength.
        
        theClients := ArrayList new.
        
        // winsock start up
        WSAStartUp22.
        
        //resolve server address and port
        #var(addrinfo)addrinfo.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_INET.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        addrinfo setFlags &int:AI_PASSIVE.
        
        #var(dirty_ptr)ptr.
        addrinfo readAddrInfo &literal:(aPort literal) &dirty_ptr:ptr.
        
        // Create a SOCKET for connecting to server
        #var(addr_reader)aReader := ptr.
        aReader read &addrinfo:addrinfo.
        
        #var(int)ai_family.
        #var(int)ai_socktype.
        #var(int)ai_protocol.
        #var(int)ai_addrlen.
        #var(dirty_ptr)ai_addrptr.
        addrinfo readInfo &vint:ai_family &vint:ai_socktype &vint:ai_protocol.
        
        theListenSocket := Socket new &int:ai_family &int:ai_socktype &int:ai_protocol.
        
        addrinfo readAddrPtr &dirty_ptr:ai_addrptr &vint:ai_addrlen.
        
        theListenSocket bind &dirty_ptr:ai_addrptr &int:ai_addrlen.
        
        aReader close.
        
        theListenSocket listen &int:SOMAXCONN.
    ]

    #method run &eachConnection:func1
    [
        theClients run &each:func1.
    ]

    #method set &blocking:aValue
    [
        #var(int)aMode.
        
        aValue 
            ? [
                aMode := 0.
            ]
            ! [
                aMode := 1.
            ].

        theListenSocket iocommand &int:FIONBIO &vint:aMode.
    ]    
    
    #method tcp_server = $self.
    
    #method set &tcp_server_listener:listener
    [
        theListener := listener.
    ]
    
    #method newClient
    [
        #var socket := theListenSocket accept.
        #var client := $nil.
        ($nil != socket)?
        [
            socket option &int:IPPROTO_TCP &int:TCP_NODELAY &bool:true.
            
            client := TcpRemoteClient new &socket:socket.
            
            theClients += client.
        ].
        
        ^ client.
    ]
    
    #method closeClient : client
        <= close &int:(theClients indexOf:client int).
    
    #method close &int:index
    [
        #var client := theClients getAt &int:index.
        
        theClients remove &int:index.
        
        client close.
                
        ($nil != theListener)
            ? [ theListener onLeave:client. ].        
    ]
    
    #method onError:e &for:client
    [
        ($nil != theListener)
            ? [ theListener onError:e &for:client. ].
        
        #var(int) err := e error_code.
        (err == WSAECONNRESET)
            ? [ $self closeClient:client. ].
    ]
    
    #method readClients
    [
        #var(int)i := 0.
        #var(int)counter := theClients length.
        
        #loop (i < counter)?
        [
            #var(tcp_remoteclient) client := theClients getAt &int:i tcp_remoteclient.
            #var(int)received.
            client read &bytearray:theBuffer &int:theMaxLength &vint:received
                | if &SocketError:e
                [
                    $self onError:e &for:client.
                ].            
            
            (received == 0)
                ? [ $self closeClient:client. ]
                ! [ 
                    (received > 0)
                        ? [ theListener onReceived:client &bytearray:theBuffer &int:received. ].
                ].
            
            i := i + 1.
        ].
    ]
    
    #method proceed
    [
        #var client := $self newClient.
        ($nil != client)?
        [
            ($nil != theListener)
                ? [ theListener onConnect:client. ].
        ].
        
        $self readClients.
        
        ^ true.
    ]
    
    #method start
    [
        theListenThread := Thread new &func:
        [
            #loop $self proceed.
        ].
        
        theListenThread start.
    ]
    
    #method(stacksafe) sendAll &bytearray:buffer &int:length
    [
        #var(int)i := 0.
        #var(int)counter := theClients length.
        
        #loop (i < counter)?
        [
            #var(tcp_remoteclient) client := theClients getAt &int:i tcp_remoteclient.
            
            client write &bytearray:buffer &int:length.
            
            i := i + 1.
        ].
    ]
    
    #method(stacksafe) sendAll &literal:s
    [
        #var(byte:256)buffer.
        #var(int)len := s length.
        #var(int)buf_len := 256.
        #var(int)index := 0.
        #loop (len > 0)?
        [
            (len < 256)
                ? [ buf_len := len. ]
                ! [ buf_len := 256. ].
            
            UTF8Encoding convert &int:index &int:buf_len &literal:s &bytearray:buffer.
            
            $self sendAll &bytearray:buffer &int:buf_len.
            
            len := len - buf_len.
            index := index + buf_len.
        ].
    ]
    
    #method(stacksafe) sendAll &int:n
    [
        #var(byte:4)buffer.
        byteArrayHelper write &bytearray:buffer &int:0 &int:n.
        
        $self sendAll &bytearray:buffer &int:4.
    ]
    
    #method(stacksafe) send &int:index &bytearray:buffer &int:length    
    [
        theClients getAt &int:index tcp_remoteclient write &bytearray:buffer &int:length.
    ]
    
    #method close
    [
        theListenSocket shutdown &int:SD_BOTH.
        
        theListenSocket close.
    ]
}