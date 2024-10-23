import system;

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
   int _code;
    
   /// Creates the exception
   constructor new(string message, int code)
      <= super new(message)
   {
      _code := code
   }
    
   constructor new(string message)
        <= super new(message)
   {
      _code := extern WS2_32.WSAGetLastError()
   }
    
   int ErrorCode = _code;
    
   /// Returns the message and the call stack
   string toPrintable()
   { 
      string retVal := message
                            .add($10"Socket Error code:")
                            .add(_code.toPrintable())
                            .add($10"Call stack:"$10)
                            .add(callStack.toPrintable());
        
      ^ retVal
   }
}

// --- WSAStartUp ---

sealed packed WSADATA
{
   short wVersion;
   short wHighVersion;
   byte  szDescription[257];
   byte  szSystemStatus[129];
   short iMaxSockets;
   short iMaxUdpDg;
   int   lpVendorInfo;
    
   constructor init(byte version, byte highVersion)
   {
      short wVersionRequested := version * 10h + highVersion;

      int retVal := extern WS2_32.WSAStartup(wVersionRequested, self);
            
      if:not (0 == retVal)
         { SocketException.new("Cannot start socket service", retVal).raise() };
            
      //startUpEvents.OnStop := { self.close() }
   }
    
   close()
   {
      extern WS2_32.WSACleanup()
   }
}

static WSAStartUp22 : preloaded = WSADATA.init(2, 2);
