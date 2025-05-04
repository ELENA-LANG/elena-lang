import system'text;
import system'collections;

public sealed class HttpHeaders
{
   List<Tuple<string, string>> _headers;

   static HttpHeaders combine(HttpHeaders headers1, HttpHeaders headers2)
   {
      auto headers := new List<Tuple<string, string>>();
      headers.appendRange(headers1.List.enumerator());
      headers.appendRange(headers2.List.enumerator());
      
      ^ HttpHeaders.assign(headers);
   }

   internal constructor assign(List<Tuple<string, string>> headers)
   {
      _headers := headers;
   }

   constructor()
   {
      _headers := new List<Tuple<string, string>>();
   }

   internal List<Tuple<string, string>> List
      = _headers;

   add(string name, string content)
   {
      _headers.append(new Tuple<string, string>(name, content));
   }

   int Length
      = _headers.Length;

   string at(string key)
   {
      foreach(auto p; in _headers) {
         if (p.Item1 == key) {   
            ^ p.Item2
         }
      };

      ^ String.MinValue
   }

   get string Value()
   {
      auto content := new StringBuilder();
      foreach(auto p; in _headers) {
         content
            .write(p.Item1)
            .write(":")
            .write(p.Item2)
            .write($13$10);         
      };

      ^ content.Value
   }
}