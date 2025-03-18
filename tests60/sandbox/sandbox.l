import extensions;

public sealed packed XSOCKADDR_IN
{
   short   family;
   short   port;
//   IN_ADDR sin_addr;
   byte    sin_zero[4]; 

   get constructor Default()
      = default;   
}

public program()
{
   XSOCKADDR_IN remote := default;
}