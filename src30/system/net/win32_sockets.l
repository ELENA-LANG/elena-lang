#import system.

// --- AddrInfo ---

#class(sealed,struct:embeddable) AddrInfo
{
    #field(int) ai_flags.
    #field(int) ai_family.
    #field(int) ai_socktype.
    #field(int) ai_protocol.
    #field(int) ai_addrlen.
    #field(dirty_ptr) ai_canonname.
    #field(dirty_ptr) ai_addr.
    #field(dirty_ptr) ai_next.
    
    #method(stacksafe)setFlags &int:aValue
    [
        ai_flags := aValue.
    ]
    
    #method(stacksafe)setFamily &int:aValue
    [
        ai_family := aValue.
    ]
    
    #method(stacksafe)setSocketType &int:aValue
    [
        ai_socktype := aValue.
    ]
    
    #method(stacksafe)setProtocol &int:aValue
    [
        ai_protocol := aValue.
    ]
    
    #method(stacksafe)setAddressLen &int:aValue
    [
        ai_addrlen := aValue.
    ]
    
    #method(stacksafe)readInfo &vint:aFamily &vint:aSocketType &vint:aProtocol
    [
        aFamily := ai_family.
        aSocketType := ai_socktype.
        aProtocol := ai_protocol.
    ]
    
    #method(stacksafe)readAddrPtr &dirty_ptr:ptr &vint:len
    [
        ptr set &dirty_ptr:ai_addr.
        len := ai_addrlen.
    ]

    #method(stacksafe)readAddrInfo &literal:aNode &literal:aService &dirty_ptr:addr
    [
        #var(int)aRetVal := system'external'WS2_32 getaddrinfo
            &literal:aNode
            &literal:aService
            &addrinfo:$self
            &dirty_ptr:addr.
            
        (0 == aRetVal)
            ! [ #throw SocketException new:"getaddrinfo failed" &error_code:(IntNumber new &int:aRetVal). ].
    ]
            
    #method(stacksafe)readAddrInfo &literal:aService &dirty_ptr:addr
    [
        #var(int)aRetVal := system'external'WS2_32 getaddrinfo
            &int:0
            &literal:aService
            &addrinfo:$self
            &dirty_ptr:addr.
            
        (0 == aRetVal)
            ! [ #throw SocketException new:"getaddrinfo failed" &error_code:(IntNumber new &int:aRetVal). ].        
    ]
            
    #method(stacksafe) clear
    [
        ai_flags := 0.
        ai_family := 0.
        ai_socktype := 0.
        ai_protocol := 0.
        ai_addrlen := 0.
        ai_canonname set &int:0.
        ai_addr set &int:0.
        ai_next set &int:0.
    ]
}

// --- AddrInfoReader ---

#subject addr_reader = AddrInfoReader.

#class(sealed,struct:embeddable) AddrInfoReader
{
    #field(dirty_ptr)paddrinfo.
    #field(dirty_ptr)prootinfo.
    
    #constructor(stacksafe) &dirty_ptr:addr
    [
        paddrinfo := addr.
        prootinfo := addr.
    ]
    
    #constructor(stacksafe) new &dirty_ptr:addr
    [
        paddrinfo := addr.
        prootinfo := addr.
    ]
    
    #method(stacksafe,bool)is &available
        = paddrinfo is &empty not.
    
    #method(stacksafe) next
    [
        #var(dirty_ptr)ptr := paddrinfo.
        ptr offset &int:28.
        
        paddrinfo := ptr.
    ]
    
    #method(stacksafe)read &addrinfo:anInfo
    [
        system'external'KERNEL32 RtlMoveMemory
            &addrinfo:anInfo
            &int:paddrinfo
            &int:32. 
    ]
    
    #method(stacksafe) close
    [
        system'external'WS2_32 freeaddrinfo
            &int:prootinfo.
    ]
    
    #method addr_reader = $self.
}

// --- Socket ---

#class(sealed,struct:reference,const)Socket
{
    #field(int)theHandle.
    
    #constructor(stacksafe) $new &int:aSocket
    [
        theHandle := aSocket.
    ]
    
    #constructor(stacksafe) new &socket:aSocket
    [
        theHandle := aSocket.
    ]
    
    #constructor(stacksafe) new &int:anAdressFamily &int:aType &int:aProtocol
        <= ($new &int:(system'external'WS2_32 socket &int:anAdressFamily &int:aType &int:aProtocol))
    [
        (INVALID_SOCKET == theHandle)
            ? [ #throw SocketException new:"Error at socket". ].
    ]
    
    #method socket = $self.
    
    #method(stacksafe) open &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        #var(int)aRetVal := system'external'WS2_32 connect 
            &int:theHandle
            &dirty_ptr:aSockAddrPtr
            &int:aNameLen.
            
        (SOCKET_ERROR == aRetVal)
            ? [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) bind &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        #var(int)aRetVal := system'external'WS2_32 bind 
            &int:theHandle
            &int:aSockAddrPtr
            &int:aNameLen.
            
        (SOCKET_ERROR == aRetVal)
            ? [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) listen &int:aBackLog
    [
        #var(int)aRetVal := system'external'WS2_32 listen 
            &int:theHandle
            &int:aBackLog.
            
        (SOCKET_ERROR == aRetVal)
            ? [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe,bool) tryOpen &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        #var(int)aRetVal := system'external'WS2_32 connect 
            &int:theHandle
            &int:aSockAddrPtr
            &int:aNameLen.
            
        ^ (SOCKET_ERROR != aRetVal).
    ]

    #method(stacksafe)write &bytearray:buffer &int:length &int:flags &vint:sent
    [
        #var(int)retVal := system'external'WS2_32 send 
            &int:theHandle
            &bytearray:buffer
            &int:length
            &int:flags.
            
        (SOCKET_ERROR == retVal)
            ? [ #throw SocketException new:"Error at socket". ].            
            
        sent := retVal.            
    ]

    #method(stacksafe) read &bytearray:buffer &int:maxLength &int:flags &vint:received
    [
        #var(int)retVal := system'external'WS2_32 recv
            &int:theHandle
            &bytearray:buffer
            &int:maxLength
            &int:flags.
        
        (SOCKET_ERROR == retVal)?
        [
            #var(int)err := system'external'WS2_32 WSAGetLastError.
            (err != WSAEWOULDBLOCK)
                ? [ #throw SocketException new:"Error at socket". ].
        ].
            
        received := retVal.    
    ]

    #method accept  
    [
        #var(socket)aSocket := system'external'WS2_32 accept
            &int:theHandle
            &int:0
            &int:0.
            
        (INVALID_SOCKET != aSocket)
            ? [
                ^ Socket new &socket:aSocket.
            ]
            ! [ ^ $nil. ].
    ]

    #method(stacksafe) shutdown &int:how
    [
        #var(int)aRetVal := system'external'WS2_32 shutdown
            &int:theHandle
            &int:how.
            
        (SOCKET_ERROR == aRetVal)
            ? [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) iocommand &int:aCommand &vint:aParameter
    [
        #var(int)aRetVal := system'external'WS2_32 ioctlsocket
            &int:theHandle
            &int:aCommand
            &vint:aParameter.
            
        (SOCKET_ERROR == aRetVal)
            ? [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) option &int:aLevel &int:anOption &bool:aValue
    [
        #var(int)n := aValue iif:1:0.
        
        #var(int)aRetVal := system'external'WS2_32 setsockopt
            &int:theHandle
            &int:aLevel
            &int:anOption
            &vint:n
            &int:1.
            
        (SOCKET_ERROR == aRetVal)
            ? [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) close
    [
        system'external'WS2_32 closesocket
            &int:theHandle.
    ]
}