import system'threading;

public interface IHttpHandlerFactory
{
   abstract IHttpHandler create(string method);
   
   abstract IHttpErrorHandler getErrorHandler();
}

public interface IHttpHandler
{
   abstract HttpServerResponse proceed(HttpServerRequest request);
   
   abstract Task<HttpServerResponse> proceedAsync(HttpServerRequest request);
}