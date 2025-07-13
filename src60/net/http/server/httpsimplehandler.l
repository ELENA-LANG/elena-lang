public class HttpSimpleGetHandler : IHttpHandler
{
   
}

public class HttpSimpleHandlerFactory : IHttpHandlerFactory
{
   IHttpHandler? create(string method)
   {
      if (method == "GET") {
         ^ new HttpSimpleGetHandler();
      };
      
      ^ nil
   }
}
