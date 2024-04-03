import system;

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
   short   family : prop;
   short   port   : prop;
   IN_ADDR sin_addr;
   byte    sin_zero[4]; 

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

   constructor()
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
   int     ai_flags;
   int     ai_family;
   int     ai_socktype;
   int     ai_protocol;
   int     ai_addrlen;
   pointer ai_canonname;
   pointer ai_addr;
   pointer ai_next;
    
   constructor()
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

   pointer Address
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
      ai_canonname := default;
      ai_addr := default;
      ai_next := default;
   }
}

// --- AddrInfoReader ---

public sealed struct AddrInfoReader
{
   pointer paddrinfo;
   pointer prootinfo;
    
   constructor(string service, AddrInfo info)
   {
      pointer ptr := default;
      int retVal := extern WS2_32.getaddrinfo(0, service, info, ref ptr);
            
      ifnot(0 == retVal)
         { SocketException.new("getaddrinfo failed", retVal).raise() };
            
      paddrinfo := ptr;
      prootinfo := ptr;
   }

   constructor(string node, string service, AddrInfo info)
   {
      pointer ptr := default;
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
      pointer ptr := paddrinfo;
      ptr := ptr + 28;
        
      int r := ptr;        
      paddrinfo := new pointer(r)
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
   handle _handle;
    
   constructor(handle handle)
   {
      _handle := handle
   }
    
   constructor(int adressFamily, int type, int protocol)
   {
      _handle := extern WS2_32.socket(adressFamily, type, protocol);
        
      if (_handle == INVALID_SOCKET)
         { SocketException.new("Error at socket").raise() };
   }
    
   open(pointer sockAddrPtr, int nameLen)
   {
      int retVal := extern WS2_32.connect( 
            _handle,
            sockAddrPtr,
            nameLen);
            
      if (SOCKET_ERROR == retVal)
         { SocketException.new("Error at socket").raise() }
   }

   bind(pointer sockAddrPtr, int nameLen)
   {
      int retVal := extern WS2_32.bind(_handle, sockAddrPtr, nameLen);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() };
   }

   listen(int backLog)
   {
      int retVal := extern WS2_32.listen(_handle, backLog);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() }
   }

   bool tryOpen(pointer ptr, int len)
   {
      int retVal := extern WS2_32.connect(_handle, ptr, len);
            
      ^ (SOCKET_ERROR != retVal)
   }

   connect(string ip_address, int port)
   {
      SOCKADDR_IN addr := new SOCKADDR_IN();
      addr.port := port;
      addr.family := system'net'AF_INET;
      addr.ip_address := ip_address;
        
      pointer ptr := addr;
      int retVal := extern WS2_32.connect(_handle, ptr, SOCKADDR_IN_SIZE);
            
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
      int retVal := extern WS2_32.send(_handle, buffer, length, flags);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() };
            
      sent := retVal
   }

   read(byte[] buffer, int maxLength, int flags, ref int recvd)
   {
      int retVal := extern WS2_32.recv(_handle,buffer,maxLength,flags);
        
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
      Socket socket := extern WS2_32.accept(_handle, 0, 0);
            
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
      int retVal := extern WS2_32.shutdown(_handle, how);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() }
   }

   iocommand(int command, ref int parameter)
   {
      int retVal := extern WS2_32.ioctlsocket(_handle, command, ref parameter);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() }
   }

   option(int level, int option, bool value)
   {
      int n := value.iif(1,0);
        
      int retVal := extern WS2_32.setsockopt(
                        _handle, level, option, ref n, 1);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() }
   }

   close()
   {
      extern WS2_32.closesocket(_handle)
   }
}