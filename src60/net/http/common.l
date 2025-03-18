import system'text'parsing;
import system'net;

public class HttpException : Exception
{
   constructor new(string s)
      <= super new(s);
}

internal static RegEx UrlRegEx = new RegEx("^(ht|f)tp(s?)\:\/\/[0-9a-zA-Z]((\.|-+)?[0-9a-zA-Z])*(:(0-9)*)?(\/?)([a-zA-Z0-9\-\.\?\,\'\/\\\+&%\$#_=]*)?$");

public sealed class Uri
{
   string _scheme;
   string _host;
   string _port;
   string _path;
   string _query;
//   string _fragment;
//   string _username;
//   string _password;

   private constructor() {}

   get string Scheme()
      = _scheme;

   get string Host()
      = _host;

   get string PortValue()
      = _port ?? String.MinValue;

   get short Port()
   {
      if:not:nil(_port) {
         ^ ShortConvertor.convert(_port, 10);
      };

      ^ 0;
   }

   get string Path()
      = _path ?? String.MinValue;

   get string Query()
      = _query ?? String.MinValue;

   constructor(string url)
      = Uri.parse(url);

   private bool parseUrl(string url)
   {
      if:not (UrlRegEx.match(url))
         { ^ false };

      int len := url.Length;
      int index := url.indexOf(0, $58);
      _scheme := url.Substring(0, index);

      // skip //
      index += 3;

      int end := url.indexOf(index, $47);
      if (end == -1)
         end := len;

      int portIndex := url.indexOf(index, $58);
      if (portIndex > 0 && portIndex < end) {
         _host := url.Substring(index, portIndex - index);
         _port := url.Substring(portIndex + 1, end - portIndex - 1);
      }
      else { 
         _host := url.Substring(index, end - index);
         _port := "80"
      };

      index := end + 1;
      
      if (index < len) {
         end := url.indexOf(index, $63);
         if (end == -1)
            end := len;

         _path := url.Substring(index, end - index);
      };

      index := end + 1;
      
      if (index < len) {
         _query := url.Substring(index, len - index);
      };

      ^ true;
   }

   constructor parse(string url)
   {
      if:not (parseUrl(url))
         InvalidArgumentException.raise("url");
   }
}