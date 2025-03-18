import system'text;

public class HttpHeaders
{
   StringBuilder _content;

   constructor()
   {
      _content := new StringBuilder();
   }

   add(string name, string content)
   {
      _content
         .write(name)
         .write(":")
         .write(content)
         .write($13$10);
   }

   get string Value()
      => _content; 
}