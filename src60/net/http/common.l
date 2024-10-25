import system'text'parsing;

internal RegEx UrlRegEx = new RegEx("^(ht|f)tp(s?)\:\/\/[0-9a-zA-Z]((\.|-+)?[0-9a-zA-Z])*(:(0-9)*)?(\/?)([a-zA-Z0-9\-\.\?\,\'\/\\\+&%\$#_]*)?$");

public sealed class Uri
{   
   private constructor() {}

   constructor(string url)
      = Uri.parse(url);

   constructor parse(url)
   {
   }
}