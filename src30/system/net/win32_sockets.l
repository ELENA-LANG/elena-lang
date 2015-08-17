#define system.

// --- AddrInfo ---

#subject(class:AddrInfo)addrinfo.

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
}

// --- AddrInfoReader ---

#subject(class:AddrInfoReader)addr_reader.

#class(sealed,struct:embeddable) AddrInfoReader
{
    #field(type:dirty_ptr)paddrinfo.
    
    #constructor new &wide:aNode &wide:aService &addrinfo:aHint
    [
        #var(type:int)aRetVal := system'external'WS2_32 GetAddrInfoW
            &wide:aNode
            &wide:aService
            &addrinfo:aHint
            &dirty_ptr:paddrinfo.
            
        (0 == aRetVal)
            ! [ #throw SocketException new:"GetAddrInfoW failed" &error_code:(IntNumber new &int:aRetVal). ].
    ]
    
    #method close
    [
        system'external'WS2_32 FreeAddrInfoW
            &int:paddrinfo.
    ]
    
    #method addr_reader = $self.
}

// --- Socket ---

#class(sealed,integer:4)Socket
{
    #constructor(stacksafe) $new &int:aSocket
        => system'internal'int_copy.
    
    #constructor(stacksafe) new &int:anAdressFamily &int:aType &int:aProtocol
        <= ($new &int:(system'external'WS2_32 socket &int:anAdressFamily &int:aType &int:aProtocol))
    [
        (INVALID_SOCKET == $self)
            ! [ #throw SocketException new:"Error at socket". ].
    ]
    
    #method socket = $self.
    
    #method close
    [
    ]
}