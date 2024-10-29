import system;
import system'threading;

// --- IN_ADDR ---

public sealed struct IN_ADDR
{
   long addr;    
    
   set Value(long addr)
   {
      this addr := addr
   }
    
   clear()
   {
      addr := 0l
   }
}

// --- SOCKADDR_IN ---

public sealed packed SOCKADDR_IN
{
   short   family : prop;
   short   port   : prop;
   IN_ADDR sin_addr;
   byte    sin_zero[4]; 

   get constructor Default()
      = default;   

   set ip_address(string ip)
   {
      long addr := extern WS2_32.inet_addr(ip);
        
      sin_addr.Value := addr
   }
}

const int SOCKADDR_IN_SIZE = 16;

// --- AddrInfo ---

public packed AddrInfo
{
   int     ai_flags     : prop;
   int     ai_family    : prop;
   int     ai_socktype  : prop;
   int     ai_protocol  : prop;
   int     ai_addrlen   : prop;
   pointer ai_canonname : prop;
   pointer ai_addr      : prop;
   pointer ai_next      : prop;
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

      if:not(0 == retVal)
         { SocketException.new("getaddrinfo failed", retVal).raise() };

      paddrinfo := ptr;
      prootinfo := ptr;
   }

   constructor(string node, string service, AddrInfo info)
   {
      pointer ptr := default;
      int retVal := extern WS2_32.getaddrinfo(node, service, info, ref ptr);
            
      if:not(0 == retVal)
         { SocketException.new("getaddrinfo failed", retVal).raise() };
            
      paddrinfo := ptr;
      prootinfo := ptr;
   }

   bool Available
      = paddrinfo.isUnassigned().Inverted;
    
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

   get constructor Default()
   {
      _handle := Handle.loadDirty(INVALID_SOCKET);
   }
    
   get bool isInvalid()
      = _handle == INVALID_SOCKET;

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
      SOCKADDR_IN addr := SOCKADDR_IN.Default;
      addr.port := port;
      addr.family := system'net'AF_INET;
      addr.ip_address := ip_address;
        
      pointer ptr := addr;
      int retVal := extern WS2_32.connect(_handle, ptr, SOCKADDR_IN_SIZE);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() }
   }

   Task connectAsync(string ip_address, int port)
      = Task.run({ connect(ip_address, port); });

   int send(byte[] buffer)
   {
      int len := buffer.Length;

      ^ self.send(buffer, len, 0);
   }

   Task<int> sendAsync(byte[] buffer)
      = ::Task<int>.run(
      {
         int written := send(buffer);

         ^ written
      });

   int send(byte[] buffer, int length, int flags)
   {
      int retVal := extern WS2_32.send(_handle, buffer, length, flags);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() };
            
      ^ retVal
   }

   Task<int> sendAsync(byte[] buffer, int length, int flags)
      = ::Task<int>.run(
      {
         int sent := send(buffer, length, flags);

         ^ sent
      });

   int receive(byte[] buffer, int maxLength, int flags)
   {
      int retVal := extern WS2_32.recv(_handle,buffer,maxLength,flags);
        
      if (retVal == SOCKET_ERROR)
      {
         int err := extern WS2_32.WSAGetLastError();
         if (err != WSAEWOULDBLOCK)
            { SocketException.new("Error at socket").raise() }
      };
            
      ^ retVal
   }

   Task<int> receiveAsync(byte[] buffer, int maxLength, int flags)
      = ::Task<int>.run(
      {
         int received := receive(buffer, maxLength, flags);

         ^ received
      });

   Socket accept()
   {
      handle socket := extern WS2_32.accept(_handle, 0, 0);
            
      ^ socket
   }

   Task<Socket> acceptAsync()
      = ::Task<Socket>.run(
      {
         handle socket := extern WS2_32.accept(_handle, 0, 0);

         ^ new Socket(socket)
      });

   shutdown(int how)
   {
      int retVal := extern WS2_32.shutdown(_handle, how);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() }
   }

   int iocommand(int command, int mode)
   {
      int parameter := mode;
      int retVal := extern WS2_32.ioctlsocket(_handle, command, ref parameter);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() };

      ^ parameter;
   }

   option(int level, int option, bool value)
   {
      int n := value ? 1 : 0;
        
      int retVal := extern WS2_32.setsockopt(
                        _handle, level, option, ref n, 1);
            
      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() }
   }

   bool isConnected()
   {      
      int size := 4;

      int retVal := extern WS2_32.getsockopt(
                        _handle, SOL_SOCKET, SO_ERROR, out int errCode, ref size);

      
      ^ (retVal == 0 && errCode == 0);
   }

   close()
   {
      extern WS2_32.closesocket(_handle)
   }
}