public const int INVALID_SOCKET = -1;
public const int SOCKET_ERROR   = -1;

public const int AI_PASSIVE  = 1;
public const int AF_UNSPEC   = 0;
public const int AF_INET     = 2;
public const int SOCK_STREAM = 1;
public const int IPPROTO_TCP = 6;

public const int TCP_NODELAY = 1;

public const int FIONBIO = 8004667Eh;

public const int WSAEWOULDBLOCK = 10035;
public const int WSAECONNRESET  = 10054;

public const int SOMAXCONN = 5;

public const int SD_BOTH = 2;

// --- WSAException ---

public class SocketException : Exception
{
    int theCode;
    
    /// Creates the exception
    constructor new(string message, int code)
        <= new(message)
    {
        theCode := code
    }
    
    constructor new(string message)
        <= new(message)
    {
        theCode := extern WS2_32.WSAGetLastError
    }
    
    int ErrorCode = theCode;
    
    /// Returns the message and the call stack
    string toPrintable()
    {
        string retVal := theMessage
                            .add($10"Socket Error code:")
                            .add(theCode.toPrintable())
                            .add($10"Call stack:"$10)
                            .add(theCallstack.toPrintable());
        
        ^ retVal
    }
}

// --- WSAStartUp ---

sealed struct WSADATA
{
    short wVersion;
    short wHighVersion;
    byte  szDescription[257];
    byte  szSystemStatus[129];
    short iMaxSockets;
    short iMaxUdpDg;
    int   lpVendorInfo;
    
    constructor init(int version, int highVersion)
        <= init(cast byte(version), cast byte(highVersion));
    
    constructor init(byte version, byte highVersion)
    {
        short wVersionRequested := version * 10h + highVersion;

        int retVal := extern WS2_32.WSAStartup(wVersionRequested, self);
            
        ifnot (0 == retVal)
            { SocketException.new("Cannot start socket service", retVal).raise() };
            
        startUpEvents.OnStop := { self.close() }
    }
    
    close()
    {
        extern WS2_32.WSACleanup()
    }
}

preloaded static WSAStartUp22 = WSADATA.init(2, 2);
