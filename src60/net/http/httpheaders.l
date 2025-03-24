import system'text;
import system'collections;

public class HttpHeaders
{
   List<Tuple<string, string>> _headers;

   constructor()
   {
      _headers := new List<Tuple<string, string>>();
   }

   add(string name, string content)
   {
      _headers.append(new Tuple<string, string>(name, content));
   }

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