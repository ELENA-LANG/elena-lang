#import system.
#import system'net.

// --- TcpClient ---

#class(sealed) TcpClient
{
    #constructor new &ip:anAddress &port:aPort
    [
        // winsock start up
        WSAStartUp22.
        
        //resolve server address and port
        #var(type:addrinfo)addrinfo.
        
        // set address info
        addrinfo clear.
        addrinfo setFamily &int:AF_UNSPEC.
        addrinfo setSocketType &int:SOCK_STREAM.
        addrinfo setProtocol &int:IPPROTO_TCP.
        
        
    ]
    
    #method tcp_client = $self.
}