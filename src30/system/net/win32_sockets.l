#define system.

// --- SocketAddress ---

#class SocketAddress
{
}

// --- Socket ---

#class(sealed,integer:4)Socket
{
    #constructor new
    [
        WSAStartUp22.
    ]
    
    #method socket = $self.
    
    #method close
    [
    ]
}