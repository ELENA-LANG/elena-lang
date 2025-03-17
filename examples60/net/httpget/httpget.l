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

singleton Tester
{
   async Task browse()
   {
      var client = new HttpClient(); 
      client.Headers.add("User-agent", "MyUserAgent");

      HttpResponse content := :await client.getAsync("http://www.google.com");

      string content := :await content.readAsStringAsync();

      console.writeLine(content);
   }
}

public program()
{
   Tester.browse().Result;

}