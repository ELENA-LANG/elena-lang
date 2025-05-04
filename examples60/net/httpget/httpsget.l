import mbedtls'registration;
import net'http;
import system'threading;

singleton Tester
{
   async Task browse(string url)
   {
      using(HttpClient client := :await HttpClient.openAsync(url))
      {
         client.Headers.add("User-agent", "MyUserAgent");

         HttpResponse response := :await client.getAsync();

         string content := :await response.readAsStringAsync();

         Console.writeLine(content);
      };
   }
}

public program()
{
   Tester.browse("https://www.google.com").Result;
}
