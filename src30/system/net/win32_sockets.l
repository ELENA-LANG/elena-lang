// ==== ver 3.4.1 ===

#import system.

// --- AddrInfo ---

#class(sealed,struct:embeddable) AddrInfo
{
    #field(type:int) ai_flags.
    #field(type:int) ai_family.
    #field(type:int) ai_socktype.
    #field(type:int) ai_protocol.
    #field(type:int) ai_addrlen.
    #field(type:dirty_ptr) ai_canonname.
    #field(type:dirty_ptr) ai_addr.
    #field(type:dirty_ptr) ai_next.
    
    #method addrinfo = $self.
    
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
    
    #method(stacksafe)readInfo &vint:aFamily &vint:aSocketType &vint:aProtocol &vint:aLen
    [
        aFamily << ai_family.
        aSocketType << ai_socktype.
        aProtocol << ai_protocol.
        aLen << ai_addrlen.
    ]
    
    #method(stacksafe)readAddrLen &vint:aLen
    [
        aLen << ai_addrlen.
    ]
    
    #method(stacksafe)readAddrPtr &dirty_ptr:aPointer
    [
        #var(type:dirty_ptr)ptr := ai_addr.
        
        aPointer write &dirty_ptr:ptr.
    ]
    
    #method(stacksafe) clear
    [
        system'external'KERNEL32 ZeroMemory
            &addrinfo:$self
            &int:32.
    ]
}

// --- AddrInfoReader ---

#subject(class:AddrInfoReader)addr_reader.

#class(sealed,struct:embeddable) AddrInfoReader
{
    #field(type:dirty_ptr)paddrinfo.
    #field(type:dirty_ptr)prootinfo.
    
    #constructor(stacksafe,embeddable) new &wide:aNode &wide:aService &addrinfo:aHint
    [
        #var(type:int)aRetVal := system'external'WS2_32 GetAddrInfoW
            &wide:aNode
            &wide:aService
            &addrinfo:aHint
            &dirty_ptr:paddrinfo.
            
        (0 == aRetVal)
            ! [ #throw SocketException new:"GetAddrInfoW failed" &error_code:(IntNumber new &int:aRetVal). ].
            
        prootinfo := paddrinfo.
    ]
    
    #method(stacksafe,type:bool)is &available
        = paddrinfo is &empty not.
    
    #method(stacksafe) next
    [
        #var(type:dirty_ptr)ptr := paddrinfo.
        ptr append &int:28.
        
        paddrinfo := ptr.
    ]
    
    #method(stacksafe)read &addrinfo:anInfo
    [
        system'external'KERNEL32 CopyMemory 
            &addrinfo:anInfo
            &int:paddrinfo
            &int:32. 
    ]
    
    #method(stacksafe) close
    [
        system'external'WS2_32 FreeAddrInfoW
            &int:prootinfo.
    ]
    
    #method addr_reader = $self.
}

// --- Socket ---

#class(sealed,struct:embeddable)Socket
{
    #field(type:int)theHandle.
    
    #constructor(stacksafe) $new &int:aSocket
        => system'internal'int_copy.
    
    #constructor(stacksafe) new &socket:aSocket
        => system'internal'int_copy.
    
    #constructor(stacksafe) new &int:anAdressFamily &int:aType &int:aProtocol
        <= ($new &int:(system'external'WS2_32 socket &int:anAdressFamily &int:aType &int:aProtocol))
    [
        (INVALID_SOCKET == $self)
            ! [ #throw SocketException new:"Error at socket". ].
    ]
    
    #method socket = $self.
    
    #method(stacksafe) open &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        #var(type:int)aRetVal := system'external'WS2_32 connect 
            &int:theHandle
            &dirty_ptr:aSockAddrPtr
            &int:aNameLen.
            
        (SOCKET_ERROR == aRetVal)
            ! [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) bind &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        #var(type:int)aRetVal := system'external'WS2_32 bind 
            &int:theHandle
            &dirty_ptr:aSockAddrPtr
            &int:aNameLen.
            
        (SOCKET_ERROR == aRetVal)
            ! [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) listen &int:aBackLog
    [
        #var(type:int)aRetVal := system'external'WS2_32 listen 
            &int:theHandle
            &int:aBackLog.
            
        (SOCKET_ERROR == aRetVal)
            ! [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe,type:bool) tryOpen &dirty_ptr:aSockAddrPtr &int:aNameLen
    [
        #var(type:int)aRetVal := system'external'WS2_32 connect 
            &int:theHandle
            &dirty_ptr:aSockAddrPtr
            &int:aNameLen.
            
        ^ (SOCKET_ERROR != aRetVal).
    ]

    #method(stacksafe)write &bytearray:aBuffer &int:aLength &int:aFlags
    [
        #var(type:int)aRetVal := system'external'WS2_32 send 
            &int:theHandle
            &bytearray:aBuffer
            &int:aLength
            &int:aFlags.
            
        (SOCKET_ERROR == aRetVal)
            ! [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) read &bytearray:aBuffer &int:aLength &int:aFlags
    [
        #var(type:int)aRetVal := system'external'WS2_32 recv
            &int:theHandle
            &bytearray:aBuffer
            &int:aLength
            &int:aFlags.
            
        (SOCKET_ERROR == aRetVal)
            ! [ #throw SocketException new:"Error at socket". ].
    ]

    #method accept
    [
        #var(type:socket)aSocket := system'external'WS2_32 accept
            &int:theHandle
            &int:0
            &int:0.
            
        (INVALID_SOCKET != aSocket)
            ? [
                ^ Socket new &socket:aSocket.
            ]
            ! [ ^ $nil. ].
    ]

    #method(stacksafe) shutdown &int:aType
    [
        #var(type:int)aRetVal := system'external'WS2_32 shutdown
            &int:theHandle
            &int:aType.
            
        (SOCKET_ERROR == aRetVal)
            ! [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) iocommand &int:aCommand &vint:aParameter
    [
        #var(type:int)aRetVal := system'external'WS2_32 ioctlsocket
            &int:theHandle
            &int:aCommand
            &vint:aParameter.
            
        (SOCKET_ERROR == aRetVal)
            ! [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) option &int:aLevel &int:anOption &bool:aValue
    [
        #var(type:int)n := aValue iif:1:0.
        
        #var(type:int)aRetVal := system'external'WS2_32 setsockopt
            &int:theHandle
            &int:aLevel
            &int:anOption
            &vint:n
            &int:1.
            
        (SOCKET_ERROR == aRetVal)
            ! [ #throw SocketException new:"Error at socket". ].
    ]

    #method(stacksafe) close
    [
        system'external'WS2_32 closesocket
            &int:theHandle.
    ]
}