import system'text;

public class HttpHeaders
{
   StringBuilder content;

   constructor()
   {
      content := new StringBuilder();
   }

   add(string name, string content)
   {
      content
         .append(name)
         .append(":")
         .append(content)
         .append($13$10);
   }

   get string Value()
      => content; 
}