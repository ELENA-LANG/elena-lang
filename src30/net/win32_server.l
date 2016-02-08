// ==== ver 3.4.1 ===

#import system.
#import system'collections.
#import system'routines.
#import system'net.
#import system'threading.

#subject(class:TcpRemoteClient)tcp_remoteclient.
#class(sealed)TcpRemoteClient
{
    #field(type:socket)theSocket.
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
        #var(type:int)sent.
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

#subject(class:TcpServerListener) tcp_server_listener.
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
    #field(type:socket)theListenSocket.
    #field(type:array_list)theClients.
    #field(type:thread)theListenThread.
    #field(type:bytearray)theBuffer.
    #field(type:int)theMaxLength.
    
    #field(type:tcp_server_listener)theListener.
    
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
        #var(type:addrinfo)addrinfo.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_INET.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        addrinfo setFlags &int:AI_PASSIVE.
        
        // Create a SOCKET for connecting to server
        #var(type:addr_reader)aReader := AddrInfoReader new &literal:(aPort literal) &addrinfo:addrinfo.        
        aReader read &addrinfo:addrinfo.
        
        #var(type:int)ai_family.
        #var(type:int)ai_socktype.
        #var(type:int)ai_protocol.
        #var(type:int)ai_addrlen.
        #var(type:dirty_ptr)ai_addrptr.
        addrinfo readInfo &vint:ai_family &vint:ai_socktype &vint:ai_protocol.
        
        theListenSocket := Socket new &int:ai_family &int:ai_socktype &int:ai_protocol.
        
        addrinfo readAddrPtr &dirty_ptr:ai_addrptr &vint:ai_addrlen.
        
        theListenSocket bind &dirty_ptr:ai_addrptr &int:ai_addrlen.
        
        aReader close.
        
        theListenSocket listen &int:SOMAXCONN.
    ]

    #method connections = theClients.

    #method set &blocking:aValue
    [
        #var(type:int)aMode.
        
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
        
        #var(type:int) err := e error_code.
        (err == WSAECONNRESET)
            ? [ $self closeClient:client. ].
    ]
    
    #method readClients
    [
        #var(type:int)i := 0.
        #var(type:int)counter := theClients length.
        
        #loop (i < counter)?
        [
            #var(type:tcp_remoteclient) client := theClients getAt &int:i tcp_remoteclient.
            #var(type:int)received.
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
        #var(type:int)i := 0.
        #var(type:int)counter := theClients length.
        
        #loop (i < counter)?
        [
            #var(type:tcp_remoteclient) client := theClients getAt &int:i tcp_remoteclient.
            
            client write &bytearray:buffer &int:length.
            
            i := i + 1.
        ].
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