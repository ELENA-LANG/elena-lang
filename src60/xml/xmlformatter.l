import system'io;

public class TabXmlFormatter : IXmlFormatter
{
   openTag(TextWriter output, int level, XmlNode node)
   {
      for (int i := 1; i < level; i++)
         output.write($9);
         
      output.write("<");
      output.write(node.Name);
      output.writeLine(">")
   }
   
   writeElement(TextWriter output, int level, XmlNode node)
   {
      for (int i := 1; i < level; i++)
         output.write($9);
         
      if:not:nil(node.Content) {
         output.write("<");
         output.write(node.Name);
         output.write(">");
         
         output.write(node.Content);
         
         output.write("</");
         output.write(node.Name);
         output.writeLine(">")         
      }
      else {
         output.write("<");
         output.write(node.Name);
         output.writeLine("/>")               
      }         
   }
   
   closeTag(TextWriter output, int level, XmlNode node)
   {
      for (int i := 1; i < level; i++)
         output.write($9);
         
      output.write("</");
      output.write(node.Name);
      output.writeLine(">")      
   }
   
}