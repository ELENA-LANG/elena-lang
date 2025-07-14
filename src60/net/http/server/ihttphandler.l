public interface IHttpHandlerFactory
{
   abstract IHttpHandler create(string method);
}

public interface IHttpHandler
{
   abstract HttpServerResponse proceed(HttpServerRequest request);
   
   abstract HttpServerResponse getErrorHandler();
}