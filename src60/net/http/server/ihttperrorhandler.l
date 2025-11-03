import system'threading;

public interface IHttpErrorHandler
{
   abstract HttpServerResponse handle(Exception ex);
   
   abstract Task<HttpServerResponse> handleAsync(Exception ex);
   
   abstract HttpServerResponse getNotFoundResponse();
   
   abstract HttpServerResponse getInternalErrorResponse(Exception ex);
}