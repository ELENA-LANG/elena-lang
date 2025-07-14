public interface IHttpErrorHandler
{
   abstract HttpServerResponse handle(Exception ex);
   
   abstract HttpServerResponse getNotFoundResponse();
   
   abstract HttpServerResponse getInternalErrorResponse(Exception ex);
}