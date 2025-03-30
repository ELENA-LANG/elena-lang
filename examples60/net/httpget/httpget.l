/*namespace HttpClientStatus;

class Program
{
    static async Task Main(string[] args)
    {
        using var client = new HttpClient();

        var result = await client.GetAsync("http://webcode.me");
        Console.WriteLine(result.StatusCode);
    }
}

using var client = new HttpClient();
var content = await client.GetStringAsync("http://webcode.me");

Console.WriteLine(content);

var url = "http://webcode.me";

using var client = new HttpClient();

var msg = new HttpRequestMessage(HttpMethod.Get, url);
msg.Headers.Add("User-Agent", "C# Program");
var res = await client.SendAsync(msg);

var content = await res.Content.ReadAsStringAsync();

Console.WriteLine(content);

*/

import net'http;
import system'threading;

singleton Tester
{
   async Task browse()
   {
      using(HttpClient client := new HttpClient())
      {
         client.Headers.add("User-agent", "MyUserAgent");

         HttpResponse response := :await client.getAsync("http://www.google.com");

         string content := :await response.readAsStringAsync();

         Console.writeLine(content);
      };
   }
}

public program()
{
   //Tester.browse().Result;
   test()
}
