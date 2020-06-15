// --- IN_ADDR ---

public sealed struct IN_ADDR
{
    long addr;    
    
    set Value(LongNumber addr)
    {
        this addr := addr
    }
    
    clear()
    {
        addr := 0l
    }
}

// --- SOCKADDR_IN ---

public sealed struct SOCKADDR_IN
{
    prop short family;
    prop short port;
    IN_ADDR    sin_addr;
    byte       sin_zero[4]; 

    set family(IntNumber n)
    {
        short n16 := n;
        
        family := n16
    }

    set port(IntNumber n)
    {
        short n16 := n;
        
        port := n16
    }

    set ip_address(string ip)
    {
        long addr := extern WS2_32.inet_addr(ip);
        
        sin_addr.Value := addr
    }

    embeddable constructor()
    {
        family := 0;
        port := 0;
        
        sin_zero[0]:=0;
        sin_zero[1]:=0;
        sin_zero[2]:=0;
        sin_zero[3]:=0;

        sin_addr.clear()
    }
}

const int SOCKADDR_IN_SIZE = 16;

// --- AddrInfo ---

public struct AddrInfo
{
    int       ai_flags;
    int       ai_family;
    int       ai_socktype;
    int       ai_protocol;
    int       ai_addrlen;
    dirty_ptr ai_canonname;
    dirty_ptr ai_addr;
    dirty_ptr ai_next;
    
    embeddable constructor()
    {
        self.clear()
    }
    
    int Flags
    {
        get()
        {
            ^ ai_flags
        }
        
        set(int val)
        {
            ai_flags := val
        }
    }
    
    int Family
    {
        get()
        {
            ^ ai_family
        }
        
        set(int val)
        {
            ai_family := val
        }
    }

    int SocketType
    {
        get()
        {
            ^ ai_socktype
        }
        
        set(int val)
        {
            ai_socktype := val
        }
    }

    int Protocol
    {
        get()
        {
            ^ ai_protocol
        }
        
        set(int val)
        {
            ai_protocol := val
        }
    }

    int AddressLen
    {
        get()
        {
            ^ ai_addrlen
        }
            
        set(int val)
        {
            ai_addrlen := val
        }
    }

    dirty_ptr Address
    {
        get()
        {
            ^ ai_addr
        }
    }
                
    clear()
    {
        ai_flags := 0;
        ai_family := 0;
        ai_socktype := 0;
        ai_protocol := 0;
        ai_addrlen := 0;
        ai_canonname := new dirty_ptr();
        ai_addr := new dirty_ptr();
        ai_next := new dirty_ptr();
    }
}

// --- AddrInfoReader ---

public sealed struct AddrInfoReader
{
    dirty_ptr paddrinfo;
    dirty_ptr prootinfo;
    
    embeddable constructor(string service, AddrInfo info)
    {
        dirty_ptr ptr := new dirty_ptr();
        int retVal := extern WS2_32.getaddrinfo(0, service, info, ref ptr);
            
        ifnot(0 == retVal)
            { SocketException.new("getaddrinfo failed", retVal).raise() };
            
        paddrinfo := ptr;
        prootinfo := ptr;
    }

    embeddable constructor(string node, string service, AddrInfo info)
    {
        dirty_ptr ptr := new dirty_ptr();
        int retVal := extern WS2_32.getaddrinfo(node, service, info, ref ptr);
            
        ifnot(0 == retVal)
            { SocketException.new("getaddrinfo failed", retVal).raise() };
            
        paddrinfo := ptr;
        prootinfo := ptr;
    }

    bool Available
        = paddrinfo.isEmpty().Inverted;
    
    next()
    {
        dirty_ptr ptr := paddrinfo;
        ptr := ptr + 28;
        
        int r := ptr.Value;
        
        paddrinfo := dirty_ptr.load(ptr.Value)
    }
    
    read(AddrInfo info)
    {
        extern KERNEL32.RtlMoveMemory(info, paddrinfo, 32)
    }
    
    close()
    {
        extern WS2_32.freeaddrinfo(prootinfo)
    }
}

// --- Socket ---

public sealed const struct Socket
{
    embeddable __raw theHandle[4];
    
    constructor(int handle)
    {
        theHandle := handle
    }
    
    constructor(int adressFamily, int type, int protocol)
    {
        theHandle := extern WS2_32.socket(adressFamily, type, protocol);
        
        if (theHandle == INVALID_SOCKET)
            { SocketException.new("Error at socket").raise() };
    }
    
    open(dirty_ptr sockAddrPtr, int nameLen)
    {
        int retVal := extern WS2_32.connect( 
            theHandle,
            sockAddrPtr,
            nameLen);
            
        if (SOCKET_ERROR == retVal)
            { SocketException.new("Error at socket").raise() }
    }

    bind(dirty_ptr sockAddrPtr, int nameLen)
    {
        int retVal := extern WS2_32.bind(theHandle, sockAddrPtr, nameLen);
            
        if (retVal == SOCKET_ERROR)
            { SocketException.new("Error at socket").raise() };
    }

    listen(int backLog)
    {
        int retVal := extern WS2_32.listen(theHandle, backLog);
            
        if (retVal == SOCKET_ERROR)
            { SocketException.new("Error at socket").raise() }
    }

    bool tryOpen(dirty_ptr ptr, int len)
    {
        int retVal := extern WS2_32.connect(theHandle, ptr, len);
            
        ^ (SOCKET_ERROR != retVal)
    }

    connect(string ip_address, int port)
    {
        SOCKADDR_IN addr := new SOCKADDR_IN();
        addr.port := port;
        addr.family := system'net'AF_INET;
        addr.ip_address := ip_address;
        
        dirty_ptr ptr := addr;
        int retVal := extern WS2_32.connect(theHandle, ptr, SOCKADDR_IN_SIZE);
            
        if (retVal == SOCKET_ERROR)
            { SocketException.new("Error at socket").raise() }
    }

    int write(byte[] buffer)
    {
        int len := buffer.Length;
        int sent := 0;
        self.write(buffer, len, 0, ref sent);
        
        ^ sent
    }

    write(byte[] buffer, int length, int flags, ref int sent)
    {
        int retVal := extern WS2_32.send(theHandle, buffer, length, flags);
            
        if (retVal == SOCKET_ERROR)
            { SocketException.new:"Error at socket".raise() };
            
        sent := retVal
    }

    read(byte[] buffer, int maxLength, int flags, ref int recvd)
    {
        int retVal := extern WS2_32.recv(theHandle,buffer,maxLength,flags);
        
        if (retVal == SOCKET_ERROR)
        {
            int err := extern WS2_32.WSAGetLastError();
            if (err != WSAEWOULDBLOCK)
                { SocketException.new("Error at socket").raise() }
        };
            
        recvd := retVal
    }

    Socket accept()
    {
        Socket socket := extern WS2_32.accept(theHandle, 0, 0);
            
        if (socket != INVALID_SOCKET)
        {
            ^ socket
        }
        else
        { 
            ^ nil 
        }
    }

    shutdown(int how)
    {
        int retVal := extern WS2_32.shutdown(theHandle, how);
            
        if (retVal == SOCKET_ERROR)
            { SocketException.new("Error at socket").raise() }
    }

    iocommand(int command, ref int parameter)
    {
        int retVal := extern WS2_32.ioctlsocket(theHandle, command, ref parameter);
            
        if (retVal == SOCKET_ERROR)
            { SocketException.new("Error at socket").raise() }
    }

    option(int level, int option, bool value)
    {
        int n := value.iif(1,0);
        
        int retVal := extern WS2_32.setsockopt(
                        theHandle, level, option, ref n, 1);
            
        if (retVal == SOCKET_ERROR)
            { SocketException.new("Error at socket").raise() }
    }

    close()
    {
        extern WS2_32.closesocket(theHandle)
    }
}