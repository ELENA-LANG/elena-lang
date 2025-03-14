public class HttpResponse
{
   NetworkStream _stream;

   internal constructor(NetworkStream stream)
   {
      _stream := stream
   }

   async Task<string> ReadAsStringAsync()
   {
      
   }
}