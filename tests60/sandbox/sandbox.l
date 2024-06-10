import extensions;

public sealed packed SOCKADDR_IN
{
   short   family ;
   short   port   ;

   get constructor Default()
      = default;   
}

public program()
{
   SOCKADDR_IN addr := SOCKADDR_IN.Default;
}
