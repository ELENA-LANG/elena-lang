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
         .write(name)
         .write(":")
         .write(content)
         .write($13$10);
   }

   get string Value()
      => content; 
}