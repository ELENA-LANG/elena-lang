import system;
import system'calendar;
import system'threading;

public const int FD_SETSIZE      = 64;

// --- TIMEVAL ---

internal sealed struct TIMEVAL
{
   int tv_sec;
   int tv_usec;

   constructor(Timespan span)
   {
      if (span.isZero()) {
         tv_sec := 0;
         tv_usec := 0;
      }
      else {
         SystemTime st := span.SystemTime;
         
         long nanosec := st.Value;
         long secs := nanosec / 1000000000l;
            
         long rem := nanosec - secs * 1000000000l;
         rem := rem / 1000l;
                            
         int tmp := IntConvertor.convert(secs);
         tv_sec := tmp;
            
         tmp := IntConvertor.convert(rem);
         tv_usec := tmp;
      }
   }
}

// --- FD_SET ---

internal sealed struct FD_SET
{
   int    fd_count : prop;

   handle fd_array[FD_SETSIZE];

   constructor()
   {
      fd_count := 0;
   }

   bool isSet(handle h)
   {
      int len := fd_count;
      for (int i := 0; i < len; i++) {
         handle current := fd_array[i];
         if (current == h) {
            ^ true;
         }
      };

      ^ false
   }
}

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
   short   family;
   short   port;
   IN_ADDR sin_addr;
   byte    sin_zero[4]; 

   get constructor Default()
      = default;   

   short Family 
   {
      get() { ^ family }

      set(val)
      {
         family := val
      }
   }

   set Ip4_address(string ip)
   {
      family := AF_INET;

      IN_ADDR in_addr := default;
      int retVal := extern WS2_32.inet_pton(AF_INET, ip, /*sin_addr*/in_addr);

      if (retVal < 0) {
         SocketException.new("Can't set remote->sin_addr.s_addr", retVal).raise();
      }
      else if (retVal == 0) {
         SocketException.new("Not a valid IP", retVal).raise();
      };

      sin_addr := in_addr;
   }
   
   set Port(short value)
   {
      port := /*extern WS2_32.htons(value)*/value;
   }

   get short Port()
   {
      short val := /*extern WS2_32.ntohs(*/port/*)*/; 

      ^ val
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

// --- SelectMode ---

public const struct SelectMode : enum<int>(SelectRead ::= 1, SelectWrite ::= 2, Error ::= 3);

// --- Socket ---

public sealed const struct Socket
{
   handle _handle;
   
   static Socket connect(string host, short port)
   {
      Socket socket := default;   

      //resolve server address and port
      AddrInfo addrinfo := default;
        
      // set address info
      addrinfo.ai_family := AF_UNSPEC;
      addrinfo.ai_socktype := SOCK_STREAM;
      addrinfo.ai_protocol := IPPROTO_TCP;
        
      AddrInfoReader reader := new AddrInfoReader(host, port.toPrintable(), addrinfo);
      while (reader.Available)
      {
         reader.read(addrinfo);
            
         int     ai_family := addrinfo.ai_family;
         int     ai_socktype := addrinfo.ai_socktype;
         int     ai_protocol := addrinfo.ai_protocol;
         int     ai_addrlen := addrinfo.ai_addrlen;
         pointer ai_addrptr := addrinfo.ai_addr;

         socket := new Socket(ai_family, ai_socktype, ai_protocol);

         if (socket.tryOpen(ai_addrptr, ai_addrlen))
         {
            reader.close();
        
            ^ socket
         };
            
         reader.next()
      };
        
      reader.close();
        
      SocketException.raise($"Unable to open {host}");

      ^ socket
   }

   static Task<Socket> connectAsync(string host, short port)
      = ::Task<Socket>.run({ ^ Socket.connect(host, port); });

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

   open(SOCKADDR_IN sockAddr)
   {
      int retVal := extern WS2_32.connect( 
            _handle,
            sockAddr,
            $size sockAddr);
            
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

   int available()
   {
      int value := 0;
      int retVal := extern WS2_32.ioctlsocket(_handle, FIONREAD, ref value);

      if (retVal == SOCKET_ERROR)
         { SocketException.new("Error at socket").raise() };

      ^ value
   }

   bool poll(Timespan timeout, SelectMode mode)
   {
      TIMEVAL timeVal := timeout;
      FD_SET fd_set := default;

      int retVal := 0;
      if (mode == SelectMode.SelectRead) {
         retVal := extern WS2_32.select(0, fd_set, 0, 0, timeVal);
      }
      else if (mode == SelectMode.SelectWrite) {
         retVal := extern WS2_32.select(0, 0, fd_set, 0, timeVal);
      }
      else if (mode == SelectMode.Error) {
         retVal := extern WS2_32.select(0, 0, 0, fd_set, timeVal);
      };

      ^ fd_set.isSet(_handle);
   }

   close()
   {
      extern WS2_32.closesocket(_handle)
   }
}