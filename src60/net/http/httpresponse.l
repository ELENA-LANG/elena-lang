import system'net;
import system'threading;

public class HttpResponse
{
   NetworkStream _stream;

   internal constructor assign(NetworkStream stream)
   {
      _stream := stream
   }

   async Task<string> ReadAsStringAsync()
   {
      
   }
}